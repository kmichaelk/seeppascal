//
// Created by Michael on 13.05.2024.
//

#ifndef SEEP_TESTS_RUNTIME_HELPER_H
#define SEEP_TESTS_RUNTIME_HELPER_H

#include "seepruntime.h"
#include <fstream>

inline std::string load_file(const std::string& path) {
    std::stringstream ss;

    std::ifstream fin(path);
    ss << fin.rdbuf();
    fin.close();

    return ss.str();
}

class ContextLeakingDebugger : public Seep::Debugger {
    std::function<void(Seep::Context& ctx)> callback;

public:
    explicit ContextLeakingDebugger(decltype(callback) callback)
        : callback(std::move(callback))
    {}

    void onAttach(Seep::Context &ctx) override {}
    void onFinish(Seep::Context &ctx) override {
        callback(ctx);
    }
};

#endif // SEEP_TESTS_RUNTIME_HELPER_H
