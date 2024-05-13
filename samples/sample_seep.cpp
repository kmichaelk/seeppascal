//
// Created by Michael on 11.05.2024.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>

#include "seepruntime.h"

using std::cout;
using std::endl;


#define ENUM_CASE(val)                                                           \
    case val: {                                                                  \
        cout << #val;    \
        break;                                                                   \
    }                                                                            \

class VariableDumpingDebugger : public Seep::Debugger {

    void onAttach(Seep::Context& ctx) override {
        cout << "--- VariableDumpingDebugger: Debugger attached\n\n";
    }

    void onFinish(Seep::Context& ctx) override {
        cout << "\n--- VariableDumpingDebugger: Execution finished\n";
        cout << "Global variables:\n";

        for (auto it = ctx.vars_begin(); it != ctx.vars_end(); ++it) {
            cout << "\t";
            cout << std::left << std::setw(10) << std::setfill(' ') << it->first;
            cout << std::left << std::setw(20) << std::setfill(' ');
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
            cout << std::right << std::setw(10) << std::setfill(' ');
            switch (it->second.type()) {
                case Seep::Type::Integer: {
                    cout << it->second.value._int;
                    break;
                }
                case Seep::Type::Real:
                    case Seep::Type::Double: {
                    cout << it->second.value._float;
                    break;
                }
                case Seep::Type::String: {
                    cout << *it->second.value._pstr;
                    break;
                }
                default: {
                    assert(!"Unhandled value");
                    break;
                }
            }
            cout << std::right << std::setw(24) << std::setfill(' ') << &*it;
            cout << "\n";
        }
    }
};

int main() {
    std::stringstream ss;

    std::ifstream fin("../examples/example1.pas");
    ss << fin.rdbuf();
    fin.close();

    //

    VariableDumpingDebugger debugger;

    Seep::Runtime runtime(ss.str());
    runtime.attach_debugger(&debugger);
    runtime.warm_up();
    runtime.execute();

    return EXIT_SUCCESS;
}