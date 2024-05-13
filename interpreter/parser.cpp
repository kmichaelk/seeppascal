#include "seepruntime.h"
#include "internals.h"

#include <antlr4-runtime.h>

#include <PascalLexer.h>
#include <PascalParser.h>

#include "evaluator.h"

class ParsingErrorListener : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token * offendingSymbol,
                     size_t line, size_t charPositionInLine, const std::string &msg,
                     std::exception_ptr e) override {

        std::ostringstream oss;
        oss << "pos: " << charPositionInLine << " - " << msg;

        throw antlr4::ParseCancellationException(oss.str());
    }
};

void Seep::Runtime::warm_up() {
    auto& frontend = internals->frontend;
    //

    new(frontend.input) antlr4::ANTLRInputStream(source);
    //
    new(frontend.lexer) ANTLRPascalParser::PascalLexer(frontend.input);
    //
    new(frontend.tokens) antlr4::CommonTokenStream(frontend.lexer);
    //
    new(frontend.parser) ANTLRPascalParser::PascalParser(frontend.tokens);
    frontend.parser->removeErrorListeners();
    frontend.parser->addErrorListener(new ParsingErrorListener());
    // frontend.parser->setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());

    //
    frontend.is_initialized = true;
}

void Seep::Runtime::execute()
{
    internals->frontend.parser->reset();

    ANTLRPascalParser::PascalParser::ProgramContext* tree;
    try {
        tree = internals->frontend.parser->program();
    }
    catch (antlr4::ParseCancellationException& e) {
        throw Seep::ParserError(e.what());
    }

    PascalEvaluator executor(*this, debugger, internals->procedures);
    executor.visit(tree);
}

void Runtime::panic(const std::string& cause) {
}
