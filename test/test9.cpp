#include <ccsh/ccsh.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

TEST(NativeTest, errorAppend)
{
    fs::path p1=ccsh::get_home()/"surethereisnofilenamedlikethis.txt";

    std::string s1;
    shell("rm",{p1.string()})>=s1;
    std::string s2=s1;
    shell("rm",{p1.string()})>>=s1;


    //std::string reqstring="surethereisnocommandnamedlikethis: command not found\n";
    //reqstring.append(reqstring);
    EXPECT_NE(s1, s2);
}
