//
// Created by Michael on 12.05.2024.
//

#ifndef SEEP_STDLIB_H
#define SEEP_STDLIB_H

#include "seepruntime.h"

namespace Seep::stdlib {
    void Write(Runtime& rt, MemRec& out, const Runtime::ArgV& args);
    void WriteLn(Runtime& rt, MemRec& out, const Runtime::ArgV& args);
    void Read(Runtime& rt, MemRec& out, const Runtime::ArgV& args);
}

#endif // SEEP_STDLIB_H
