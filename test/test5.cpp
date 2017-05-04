#define CCSH_NON_STANDARD_DOLLAR

#include <ccsh/ccsh.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;


TEST(WrapperTest, dollar)
{
    std::string str="test";

    dollar("VARNATIVE")=str;
    ccsh::$("VARWRAPPER")=str;

    std::string natstr=dollar("VARNATIVE");
    std::string wrapstr=$("VARWRAPPER");
    EXPECT_EQ(natstr, wrapstr);
}
