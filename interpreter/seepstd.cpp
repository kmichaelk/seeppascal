//
// Created by Michael on 12.05.2024.
//

#include "seepstd.h"

using namespace Seep;

void stdlib::Write(Runtime& rt, MemRec& out, const Runtime::ArgV& args) {
    for (const auto& rec : args) {
        switch (rec->type()) {
            case Type::Integer: {
                rt.out << rec->value._int;
                break;
            }
            case Type::Double: {
                rt.out << rec->value._float;
                break;
            }
            case Type::String: {
                rt.out << *rec->value._pstr;
                break;
            }
            default: {
                throw std::runtime_error("TODO");
            }
        }
    }
}

void stdlib::WriteLn(Runtime& rt, MemRec& out, const Runtime::ArgV& args) {
    Write(rt, out, args);
    rt.out << "\n";
}

void stdlib::Read(Runtime& rt, MemRec& out, const Runtime::ArgV& args) {
    auto& rec = *args[0];
    switch (rec.type()) {
        case Type::Integer: {
            rt.in >> rec.value._int;
            break;
        }
        case Type::Double: {
            rt.in >> rec.value._float;
            break;
        }
        case Type::String: {
            rt.in >> *rec.value._pstr;
            break;
        }
        default: {
            throw std::runtime_error("TODO");
        }
    }
}

// #include <cstdarg>
// void stdlib::Write(Runtime& rt, void *out, ...) {
//     va_list args;
//     va_start(args, out);
//     //
//     const auto* str = va_arg(args, std::string*);
//     rt->out << *str;
//     //
//     va_end(args);
// }
