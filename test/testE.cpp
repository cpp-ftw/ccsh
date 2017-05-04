#include <ccsh/ccsh.hpp>
#include <ccsh/core.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

TEST(WrapperTest, inMapping)
{
    std::string s1, str_in="test";

    cat()<str_in>s1;

    EXPECT_EQ(str_in, "");
    EXPECT_EQ(s1, "test");
}
