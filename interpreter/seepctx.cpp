//
// Created by Michael on 12.05.2024.
//

#include "seepruntime.h"
#include "seepctx.h"

Seep::Context::Context(State* state)
    : state(state)
{}

Seep::Runtime&  Seep::Context::get_runtime() const {
    return *state->runtime;
}

std::unordered_map<std::string, Seep::MemRec>::iterator Seep::Context::vars_begin() const {
    return state->globals->begin();
}
std::unordered_map<std::string, Seep::MemRec>::iterator Seep::Context::vars_end() const {
    return state->globals->end();
}
