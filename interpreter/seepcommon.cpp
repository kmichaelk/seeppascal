//
// Created by Michael on 13.05.2024.
//

#include "seepruntime.h"

bool Seep::BelongsToGroup(Type type, TypeGroup group) noexcept {
    switch (group) {
        case TypeGroup::Reference: return type == Type::Reference;

        case TypeGroup::Any:
        case TypeGroup::Variadic: return true;

        case TypeGroup::Numeric: return
            type == Type::Char
            || type == Type::ShortInt
            || type == Type::Byte
            || type == Type::Integer
            || type == Type::Word
            || type == Type::LongInt
            ;

        case TypeGroup::Real: return
            type == Type::Real
            || type == Type::Single
            || type == Type::Double
            || type == Type::Extended
            ;

        case TypeGroup::String: return type == Type::String;
    }
    assert(false);
    return false;
}
