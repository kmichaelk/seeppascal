//
// Created by Michael on 11.05.2024.
//

#ifndef SEEP_INTERPRETER_EXECUTOR_H
#define SEEP_INTERPRETER_EXECUTOR_H

#include "seepruntime.h"
#include "seepctx.h"
#include "seepfun.h"
#include "memmod.h"

#include <functional>
#include <stdexcept>

#include <PascalParserBaseVisitor.h>

using namespace Seep;
using namespace ANTLRPascalParser;

const std::map<std::string, Type> TYPE_MAPPING = {
    { "boolean", Type::Boolean },
    { "char", Type::Char },

    { "integer", Type::Integer },

    { "real", Type::Real },
    { "double", Type::Double },

    { "string", Type::String },
};

const std::map<std::string, std::function<bool(const MemRec&, const MemRec&)>> CMP_MAPPING = {
    { "=",  std::equal_to<>{} },
    { "<>", std::not_equal_to<>{} },
    { "<",  std::less<>{} },
    { "<=", std::less_equal<>{} },
    { ">=", std::greater_equal<>{} },
    { ">",  std::greater<>{} },
};

class PascalEvaluator : public PascalParserBaseVisitor
{
    Runtime& rt;
    Debugger* dbg;

    Context::State rt_state;
    Context rt_ctx;

    Scope globals;
    std::stack<MemRec> st;
    std::stack<size_t> sp;

    const ProcStore& procedures;

    void st_save() {
        sp.push(st.size());
    }
    void st_pop() {
        const auto sz = sp.top();
        sp.pop();

        const auto diff = st.size() - sz;

        for (size_t i = 0; i < diff; i++) {
            st.pop();
        }
    }

    template<class... Args>
    MemRec* st_alloc(Args&&... args) {
        st.emplace(std::forward<Args>(args)...);
        return &st.top();
    }

public:

    explicit PascalEvaluator(Runtime& runtime, Debugger* debugger, const ProcStore& procedures)
        : rt(runtime)
        , dbg(debugger)
        , rt_state({ &runtime, &globals })
        , rt_ctx(&rt_state)
        , procedures(procedures) {
    }

    std::any visitProgram(PascalParser::ProgramContext *ctx) override {
        visitProgramHeading(ctx->programHeading());

        if (dbg) {
            dbg->onAttach(rt_ctx);
        }

        visitBlock(ctx->block());

        if (dbg) {
            dbg->onFinish(rt_ctx);
        }
        return {};
    }

    std::any visitProgramHeading(PascalParser::ProgramHeadingContext *ctx) override {
        rt_ctx.state->program_name = ctx->identifier()->getText();

        return {};
    }

    std::any visitBlock(PascalParser::BlockContext *ctx) override {
        for (const auto& part : ctx->constantDefinitionPart()) {
            for (const auto& def : part->constantDefinition()) {
                const auto& name = def->identifier()->getText();
                const auto val = std::stod(def->constant()->getText());
                globals.bind(name, MemRec(std::any_cast<Type>(visitType_(def->type_())), &val));
            }
        }
        for (const auto& part : ctx->variableDeclarationPart()) {
            for (const auto& decl : part->variableDeclaration()) {
                const auto type = std::any_cast<Type>(visitType_(decl->type_()));
                for (const auto& identifier : decl->identifierList()->identifier()) {
                    const auto& name = identifier->getText();
                    globals.bind(name, MemRec(type, nullptr));
                }
            }
        }

        visitCompoundStatement(ctx->compoundStatement());

        return {};
    }

    std::any visitType_(PascalParser::Type_Context *ctx) override {
        auto* simpleType = ctx->simpleType();
        if (!simpleType) {
            throw std::runtime_error("Not implemented");
        }
        return visitSimpleType(simpleType);
    }

    std::any visitSimpleType(PascalParser::SimpleTypeContext *ctx) override {
        const auto it = TYPE_MAPPING.find(ctx->getText());
        if (it == TYPE_MAPPING.end()) {
            throw ParserError("Unsupported type");
        }
        return it->second;
    }

    std::any visitAssignmentStatement(PascalParser::AssignmentStatementContext *ctx) override {
        const auto& var = ctx->variable()->getText();
        const auto expr = ctx->expression();

        st_save();

        auto val = std::any_cast<MemRec*>(visitExpression(expr));

        auto& rec = globals[var];
        if (rec.type() != val->type()) {
            if (rec.type() == Type::Integer && val->type() == Type::Double) {
                const auto v = static_cast<int>(val->value._float);
                rec.reassign(&v);
            } else if (rec.type() == Type::Double && val->type() == Type::Integer) {
                const auto v = static_cast<double>(val->value._int);
                rec.reassign(&v);
            } else {
                throw std::runtime_error("Assignment type mismatch");
            }
        }
        rec = std::move(*val);
        st_pop();

        return {};
    }

    std::any visitExpression(PascalParser::ExpressionContext *ctx) override {
        if (!ctx->relationaloperator()) {
            return visitSimpleExpression(ctx->simpleExpression());
        }

        st_save();

        const auto& lhs = std::any_cast<MemRec*>(visitSimpleExpression(ctx->simpleExpression())/*visit(ctx->children[0])*/);
        const auto& rhs = std::any_cast<MemRec*>(visitExpression(ctx->expression())/*visit(ctx->children[2])*/);

        const auto it = CMP_MAPPING.find(ctx->relationaloperator()->getText());
        if (it == CMP_MAPPING.end()) {
            throw ParserError("Unknown binary operator");
        }

        const bool val = it->second(*lhs, *rhs);

        auto res = MemRec(Type::Boolean, &val);

        st_pop();

        return st_alloc(std::move(res));
    }

    std::any visitSimpleExpression(PascalParser::SimpleExpressionContext *ctx) override {
        if (!ctx->additiveoperator()) {
            return visitTerm(ctx->term());
        }

        st_save();

        const auto lhs = std::any_cast<MemRec*>(visitTerm(ctx->term()));
        const auto rhs = std::any_cast<MemRec*>(visitSimpleExpression(ctx->simpleExpression()));

        MemRec res;
        if (auto* op = ctx->additiveoperator(); op->PLUS()) {
            res = *lhs + *rhs;
            goto ret;
        }
        else if (op->MINUS()) {
            res = *lhs - *rhs;
            goto ret;
        }
        else if (op->OR()) {
            throw std::runtime_error("TODO");
        }

        throw std::runtime_error("Unsupported binary operator");

    ret:
        st_pop();
        return st_alloc(std::move(res));
    }

    std::any visitTerm(PascalParser::TermContext *ctx) override {
        if (ctx->children.size() == 1) {
            return visitSignedFactor(ctx->signedFactor());
        }

        st_save();

        const auto& lhs = std::any_cast<MemRec*>(visitSignedFactor(ctx->signedFactor()));
        const auto& rhs = std::any_cast<MemRec*>(visitTerm(ctx->term()));

        MemRec res;

        if (auto* op = ctx->multiplicativeoperator(); op->STAR()) {
            res = *lhs * *rhs;
            goto ret;
        }
        else if (op->SLASH()) {
            res = *lhs / *rhs;
            goto ret;
        }
        else if (op->DIV()) {
            res = *lhs / *rhs;
            if (res.type() != Type::Integer) {
                res = MemRec(Type::Integer, &res.value._float);
            }
            goto ret;
        }
        else if (op->MOD()) {
            res = *lhs % *rhs;
            goto ret;
        }
        else if (op->AND()) {
            throw std::runtime_error("&&");
            // return lhs && rhs;
        }

        throw std::runtime_error("Unsupported operator");

    ret:
        st_pop();
        return st_alloc(std::move(res));
    }

    std::any visitFactor(PascalParser::FactorContext *ctx) override {
        if (auto* expr = ctx->expression()) {
            return visitExpression(expr);
        }
        return visitChildren(ctx);
    }

    std::any visitSignedFactor(PascalParser::SignedFactorContext *ctx) override {
        st_save();

        const auto& val = std::any_cast<MemRec*>(visitFactor(ctx->factor()));

        if (ctx->MINUS()) {
            auto res = -*val;
            st_pop();
            return st_alloc(std::move(res));
        }

        return val;
    }

    std::any visitUnsignedInteger(PascalParser::UnsignedIntegerContext *ctx) override {
        const auto val = std::stoi(ctx->getText());
        return st_alloc(Type::Integer, &val);
    }

    std::any visitUnsignedReal(PascalParser::UnsignedRealContext *ctx) override {
        const auto val = std::stod(ctx->getText());
        return st_alloc(Type::Double, &val);
    }

    std::any visitString(PascalParser::StringContext *ctx) override {
        const auto val = ctx->getText();
        return st_alloc(Type::String, &val);
    }

    std::any visitVariable(PascalParser::VariableContext *ctx) override {
        return &globals[ctx->identifier(0)->getText()];
    }

    std::any visitProcedureStatement(PascalParser::ProcedureStatementContext *ctx) override {
        const auto& name = ctx->identifier()->getText();

        const auto it = procedures.find(name);
        if (it == procedures.end()) {
            throw std::runtime_error("Procedure is not bound");
        }

        const auto& proc = it->second;
        auto args = std::any_cast<std::vector<MemRec*>>(visitParameterList(ctx->parameterList()));

        const size_t argc = proc.arg_types.size();

        for (size_t i = 0, j = 0; i < argc; i++) {
            if (proc.arg_types[i] == TypeGroup::Reference) {
                continue;
            }
            if (proc.arg_types[i] == TypeGroup::Variadic) {
                break;
            }
            if (!BelongsToGroup(args[j]->type(), proc.arg_types[i])) {
                throw std::runtime_error("Wrong argument type");
            }
            j++;
        }

        Runtime::ArgV argv;
        bool ref = false;

        std::stack<MemRec> frame; // could use vector, but its pointers can get invalidated on insertion

        for (size_t i = 0, j = 0; i < argc; i++) {
            if (proc.arg_types[i] == TypeGroup::Reference) {
                ref = true;
                continue;
            }
            if (proc.arg_types[i] == TypeGroup::Variadic) {
                for (; j < args.size(); j++) {
                    if (ref) {
                        argv.push_back(args[j]);
                        ref = false;
                    } else {
                        argv.push_back(&frame.emplace(*args[j]));
                    }
                }
                break;
            }

            if (ref) {
                argv.push_back(args[j]);
                ref = false;
            } else {
                argv.push_back(&frame.emplace(*args[j]));
            }

            j++;
        }

        MemRec res(proc.ret_type);
        //
        proc.pfn(rt, res, argv);
        //
        return res;
    }

    std::any visitParameterList(PascalParser::ParameterListContext *ctx) override {
        std::vector<MemRec*> params;
        for (const auto& param : ctx->actualParameter()) {
            params.push_back(std::any_cast<MemRec*>(visitActualParameter(param)));
        }
        return params;
    }

    std::any visitIfStatement(PascalParser::IfStatementContext *ctx) override {
        st_save();
        //
        const bool result = std::any_cast<MemRec*>(visitExpression(ctx->expression()))->value._bool;
        //
        st_pop();

        if (result) {
            visitStatement(ctx->statement(0));
        } else {
            visitStatement(ctx->statement(1));
        }

        return {};
    }

    std::any visitWhileStatement(PascalParser::WhileStatementContext *ctx) override {
        while (true) {
            st_save();
            //
            const bool result = std::any_cast<MemRec*>(visitExpression(ctx->expression()))->value._bool;
            //
            st_pop();

            if (!result) {
                break;
            }

            visitStatement(ctx->statement());
        }

        return {};
    }

    std::any visitForStatement(PascalParser::ForStatementContext *ctx) override {
        auto& var = globals[ctx->identifier()->getText()];

        if (!BelongsToGroup(var.type(), TypeGroup::Numeric)) {
            throw std::runtime_error("Only integers applicable as variables in for loop");
        }

        st_save();
        //
        var.value._int  = std::any_cast<MemRec*>(visitExpression(ctx->forList()->initialValue()->expression()))->value._int;
        int final_value = std::any_cast<MemRec*>(visitExpression(ctx->forList()->finalValue()->expression()))->value._int;
        //
        st_pop();

        while (var.value._int < final_value) {
            visitStatement(ctx->statement());
            var.value._int++;
        }
    }
};

#endif // SEEP_INTERPRETER_EXECUTOR_H
