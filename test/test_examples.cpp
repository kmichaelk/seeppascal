//
// Created by Michael on 13.05.2024.
//

#include <gtest.h>
#include <sstream>
#include "seepruntime.h"
#include "rt_helper.h"

TEST(Seep_Examples, BuiltinsRefs) {
    ContextLeakingDebugger debugger([](Seep::Context& ctx) {
        const Seep::MemRec* rec;

        ASSERT_NO_THROW(rec = &ctx.vars().at("n"));
        EXPECT_EQ(Seep::Type::Integer, rec->type());
        EXPECT_EQ(5, rec->value._int);
    });

    std::stringstream ss;
    ss << 5;
    ss << '\n';
    ss.flush();

    Seep::Runtime rt(load_file("../../examples/builtins_refs.pas"), ss, ss);

    ASSERT_NO_THROW(rt.warm_up());
    ASSERT_NO_THROW(rt.execute());
}

TEST(Seep_Examples, WhileLoops) {
    ContextLeakingDebugger debugger([](Seep::Context& ctx) {
        const Seep::MemRec* rec;

        ASSERT_NO_THROW(rec = &ctx.vars().at("i"));
        EXPECT_EQ(Seep::Type::Integer, rec->type());
        EXPECT_EQ(10, rec->value._int);
    });

    std::stringstream ss;

    Seep::Runtime rt(load_file("../../examples/while_loops.pas"), ss, ss);

    ASSERT_NO_THROW(rt.warm_up());
    ASSERT_NO_THROW(rt.execute());
}

TEST(Seep_Examples, example1) {
    ContextLeakingDebugger debugger([](Seep::Context& ctx) {
        const Seep::MemRec* rec;

        ASSERT_NO_THROW(rec = &ctx.vars().at("Pi"));
        EXPECT_EQ(Seep::Type::Double, rec->type());
        EXPECT_TRUE(fabs(rec->value._float - 3.1415926) < 0.01);

        ASSERT_NO_THROW(rec = &ctx.vars().at("num1"));
        EXPECT_EQ(Seep::Type::Integer, rec->type());
        EXPECT_EQ(5, rec->value._int);

        ASSERT_NO_THROW(rec = &ctx.vars().at("num2"));
        EXPECT_EQ(Seep::Type::Integer, rec->type());
        EXPECT_EQ(20, rec->value._int);

        ASSERT_NO_THROW(rec = &ctx.vars().at("d"));
        EXPECT_EQ(Seep::Type::Double, rec->type());
        EXPECT_EQ(2, rec->value._float);

        ASSERT_NO_THROW(rec = &ctx.vars().at("Res"));
        EXPECT_EQ(Seep::Type::Double, rec->type());
        EXPECT_TRUE(fabs(rec->value._float - 1.25) < 0.001);
    });

    std::stringstream ss;
    ss << 20;
    ss << '\n';
    ss << 2;
    ss << '\n';
    ss.flush();

    Seep::Runtime rt(load_file("../../examples/example1.pas"), ss, ss);

    ASSERT_NO_THROW(rt.warm_up());
    ASSERT_NO_THROW(rt.execute());
}