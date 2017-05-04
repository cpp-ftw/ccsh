#include <ccsh/ccsh.hpp>
#include <ccsh/core.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

TEST(WrapperTest, ls_wc_arg)
{
    std::string natstr, wrapstr, natwrapstr;

    (shell("ls",{"-l"})|shell("wc", {"-l"}))>natstr;

    (ls().l()|wc().l())>wrapstr;

    EXPECT_EQ(natstr, wrapstr);
}
