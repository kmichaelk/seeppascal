//
// Created by Michael on 12.05.2024.
//

#include "seepruntime.h"
#include "seepctx.h"

Seep::Context::Context(State* state)
    : state(state)
{}

Seep::Runtime& Seep::Context::get_runtime() const {
    return *state->runtime;
}

const std::string& Seep::Context::get_program_name() const {
    return state->program_name;
}

const std::unordered_map<std::string, Seep::MemRec>& Seep::Context::vars() const {
    return state->globals->storage();
}
