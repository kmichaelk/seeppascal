//
// Created by Michael on 11.05.2024.
//

#ifndef SEEP_MEMORY_MODEL_H
#define SEEP_MEMORY_MODEL_H

#include "seepruntime.h"

#include <unordered_map>
#include <string>

class Scope {
    using TName = std::string;

    Scope* parent;
    std::unordered_map<TName, Seep::MemRec> values;
public:
    explicit Scope(Scope* parent = nullptr)
        : parent(parent)
    {}

    Seep::MemRec& operator[](const TName& name) {
        for (auto* scope = this; scope != nullptr; scope = scope->parent) {
            auto pair = scope->values.find(name);
            if (pair == scope->values.end()) {
                continue;
            }
            return pair->second;
        }

        throw std::runtime_error("Variable is not bound");
    }

    void bind(const std::string& name, Seep::MemRec&& value) {
        values[name] = std::move(value);
    }

    auto begin() {
        return values.begin();
    }
    auto end() {
        return values.end();
    }
};

#endif // SEEP_MEMORY_MODEL_H
