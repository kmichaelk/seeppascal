//
// Created by Michael on 11.05.2024.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#include "seepruntime.h"
#include "vdd.h"

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char** argv) {
    std::string path;

    if (argc != 2) {
        path = "../examples/example1.pas";
        // cerr << "Usage: ./sample_seep [file]" << endl;
        // return EXIT_FAILURE;
    } else {
        path = argv[1];
    }

    std::stringstream ss;

    std::ifstream fin(path);
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