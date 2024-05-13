//
// Created by Michael on 13.05.2024.
//

#include "memmod.h"

Scope::Scope(Scope* parent)
    : parent(parent)
{}

Seep::MemRec& Scope::operator[](const name_t &name) {
    for (auto* scope = this; scope != nullptr; scope = scope->parent) {
        auto pair = scope->values.find(name);
        if (pair == scope->values.end()) {
            continue;
        }
        return pair->second;
    }

    throw std::runtime_error("Variable is not bound");
}

void Scope::bind(const std::string& name, Seep::MemRec&& value) {
    values[name] = std::move(value);
}

void Scope::set_writable(bool writable) {
    this->writable = writable;
}

const decltype(Scope::values)& Scope::storage() const {
    return values;
}

Scope* Scope::up() const {
    return parent;
}