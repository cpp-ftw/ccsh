#include <ccsh/ccsh.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

TEST(NativeTest, errorRedirection)
{
    fs::path p1=ccsh::get_home()/"surethereisnofilenamedlikethis.txt";

    std::string s1;
    shell("rm",{p1.string()})>=s1;

    std::string reqstring="rm: cannot remove '";
    reqstring.append(p1.string());
    reqstring.append("': No such file or directory\n");
    EXPECT_EQ(s1, reqstring);
}
