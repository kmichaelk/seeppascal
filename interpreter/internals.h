//
// Created by Michael on 11.05.2024.
//

#ifndef SEEP_INTERNALS_H
#define SEEP_INTERNALS_H

#include "seepruntime.h"
#include "seepfun.h"

#include <antlr4-runtime.h>
#include <PascalLexer.h>
#include <PascalParser.h>

struct Frontend {

    antlr4::ANTLRInputStream* const input = reinterpret_cast<antlr4::ANTLRInputStream*>((std::byte*)mem + 0);
    ANTLRPascalParser::PascalLexer* const lexer = reinterpret_cast<ANTLRPascalParser::PascalLexer*>((std::byte*)input + sizeof(*input));
    antlr4::CommonTokenStream* const tokens = reinterpret_cast<antlr4::CommonTokenStream*>((std::byte*)lexer + sizeof(*lexer));
    ANTLRPascalParser::PascalParser* const parser = reinterpret_cast<ANTLRPascalParser::PascalParser*>((std::byte*)tokens + sizeof(*tokens));

private:
    std::byte mem[0 + sizeof(*input) + sizeof(*lexer) + sizeof(*tokens) + sizeof(*parser)];

public:
    bool is_initialized = false;

    Frontend() = default;
    Frontend(const Frontend& other) = delete;
    Frontend& operator=(const Frontend& other) = delete;

    ~Frontend() {
        if (!is_initialized) return;

        // std::destroy_at<T>(obj) <-> obj->~T()
        std::destroy_at(input);
        std::destroy_at(lexer);
        std::destroy_at(tokens);
        std::destroy_at(parser);
    }
};

struct Seep::Runtime::Internals {
    Frontend frontend;
    ProcStore procedures;
};

#endif // SEEP_INTERNALS_H
