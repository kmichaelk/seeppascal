//
// Created by Michael on 12.05.2024.
//

#ifndef SEEP_CONTEXT_H
#define SEEP_CONTEXT_H

#include "memmod.h"

struct Seep::Context::State {
    Runtime* runtime;
    Scope* globals;
    std::string program_name;
};

#endif // SEEP_CONTEXT_H
