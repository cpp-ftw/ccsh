#include <ccsh/ccsh.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

TEST(NativeTest, dollar)
{
    std::string str="test";

    dollar("TESTVAR")=str;

    std::string gotstr=dollar("TESTVAR");
    EXPECT_EQ(str, gotstr);
}
