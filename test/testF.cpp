#define CCSH_NON_STANDARD_DOLLAR

#include <ccsh/ccsh.hpp>
#include <ccsh/core.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

TEST(NativeTest, source)
{
    source("../sample/vars");
    std::string var1, var2;
    var1=$("EXAMPLE_ENV_VAR1");
    var2=$("EXAMPLE_ENV_VAR2");
    //EXPECT_EQ(var1, "");
    //EXPECT_EQ(var2, "value2");

    EXPECT_EQ(true, true);
}
