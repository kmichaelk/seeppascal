//
// Created by Michael on 11.05.2024.
//

#include "seepruntime.h"
#include "internals.h"
#include "seepstd.h"

#include <utility>

using namespace Seep;

Runtime::Runtime(std::string source, std::ostream& out, std::istream& in)
        : internals(new Internals)
        , source(std::move(source))
        , debugger(nullptr)
        , in(in)
        , out(out) {

    bind_procedure("Write", { TypeGroup::Variadic }, Type::Void, stdlib::Write);
    bind_procedure("WriteLn", { TypeGroup::Variadic }, Type::Void, stdlib::WriteLn);
    bind_procedure("Read", { TypeGroup::Reference, TypeGroup::Any }, Type::Void, stdlib::Read);
}

Runtime::~Runtime() {
    delete internals;
}

void Runtime::bind_procedure(const std::string& name, const std::vector<TypeGroup>& arg_types, Type ret_type, Procedure pfn) {
    internals->procedures[name] = { arg_types, ret_type, std::move(pfn) };
}

void Runtime::attach_debugger(Seep::Debugger* _debugger) {
    debugger = _debugger;
}
