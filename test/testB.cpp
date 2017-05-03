#include <ccsh/ccsh.hpp>
#include <ccsh/core.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

TEST(WrapperTest, ls_wc)
{
    std::string natstr, wrapstr, natwrapstr;

    (shell("ls",{})|shell("wc", {"-l"}))>natstr;

    (ls()|shell("wc", {"-l"}))>natwrapstr;

    (ls()|wc("-l"))>wrapstr;

    EXPECT_EQ(natstr, wrapstr);
    EXPECT_EQ(wrapstr, natwrapstr);
}
