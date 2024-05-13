//
// Created by Michael on 13.05.2024.
//

#ifndef SEEP_VARDUMPDBG_H
#define SEEP_VARDUMPDBG_H

#include <iostream>
#include <iomanip>

#define ENUM_CASE(val)          \
    case val: {                 \
        std::cout << #val;           \
        break;                  \
    }                           \

class VariableDumpingDebugger : public Seep::Debugger {

    void onAttach(Seep::Context& ctx) override {
        std::cout << "--- VariableDumpingDebugger: Debugger attached\n\n";
    }

    void onFinish(Seep::Context& ctx) override {
        std::cout << "\n--- VariableDumpingDebugger: Execution finished\n";
        std::cout << "Global variables:\n";

        for (auto it = ctx.vars_begin(); it != ctx.vars_end(); ++it) {
            std::cout << "\t";
            std::cout << std::left << std::setw(10) << std::setfill(' ') << it->first;
            std::cout << std::left << std::setw(20) << std::setfill(' ');
            switch (it->second.type()) {
                ENUM_CASE(Seep::Type::Boolean)
                ENUM_CASE(Seep::Type::Char)
                ENUM_CASE(Seep::Type::ShortInt)
                ENUM_CASE(Seep::Type::Byte)
                ENUM_CASE(Seep::Type::Integer)
                ENUM_CASE(Seep::Type::Word)
                ENUM_CASE(Seep::Type::LongInt)
                ENUM_CASE(Seep::Type::Real)
                ENUM_CASE(Seep::Type::Single)
                ENUM_CASE(Seep::Type::Double)
                ENUM_CASE(Seep::Type::Extended)
                ENUM_CASE(Seep::Type::String)
                default: {
                    assert(!"Unhandled type");
                    break;
                }
            }
            std::cout << std::right << std::setw(10) << std::setfill(' ');
            switch (it->second.type()) {
                case Seep::Type::Integer: {
                    std::cout << it->second.value._int;
                    break;
                }
                case Seep::Type::Real:
                    case Seep::Type::Double: {
                    std::cout << it->second.value._float;
                    break;
                }
                case Seep::Type::String: {
                    std::cout << *it->second.value._pstr;
                    break;
                }
                default: {
                    assert(!"Unhandled value");
                    break;
                }
            }
            std::cout << std::right << std::setw(24) << std::setfill(' ') << &*it;
            std::cout << "\n";
        }
    }
};

#undef ENUM_CASE

#endif // SEEP_VARDUMPDBG_H
