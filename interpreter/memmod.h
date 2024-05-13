//
// Created by Michael on 11.05.2024.
//

#ifndef SEEP_MEMORY_MODEL_H
#define SEEP_MEMORY_MODEL_H

#include "seepruntime.h"

#include <unordered_map>
#include <string>

class Scope {
    using name_t = std::string;

    Scope* parent;
    std::unordered_map<name_t, Seep::MemRec> values;
    bool writable = true;
public:
    explicit Scope(Scope* parent = nullptr);

    [[nodiscard]] Seep::MemRec& operator[](const name_t& name);
    void bind(const std::string& name, Seep::MemRec&& value);

    void set_writable(bool writable);

    [[nodiscard]] const decltype(values)& storage() const;
    [[nodiscard]] Scope* up() const;
};

#endif // SEEP_MEMORY_MODEL_H
