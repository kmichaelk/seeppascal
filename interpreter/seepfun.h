//
// Created by Michael on 13.05.2024.
//

#ifndef SEEP_FUNCTIONAL_H
#define SEEP_FUNCTIONAL_H

#include "seepruntime.h"

#include <vector>
#include <map>

struct FunctionInfo {
    std::vector<Seep::TypeGroup> arg_types;
    Seep::Type ret_type;
    Seep::Runtime::Procedure pfn;
};

using ProcStore = std::map<std::string, FunctionInfo>;

#endif // SEEP_FUNCTIONAL_H
