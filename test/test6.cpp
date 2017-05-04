#include <ccsh/ccsh.hpp>
#include <ccsh/core.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

std::string ReadAllText3(fs::path const& filename)
{
    std::ifstream t(filename.c_str());
    return std::string(std::istreambuf_iterator<char>(t),
                       std::istreambuf_iterator<char>());
}

TEST(NativeTest, append)
{
    fs::path p1=ccsh::get_home()/"nofilenamedlikethis.txt";
    fs::path p2=ccsh::get_home()/"nofilenamedlikethis2.txt";

    std::string s1, s2;

    shell("rm",{p1.string()})>=p2;
    shell("rm",{p1.string()})>>=p2;
    EXPECT_EQ(fs::exists(p1), false);

    (shell("wc",{"-l"})<p2>s1).run();
    EXPECT_EQ(s1, "2\n");

    (shell("echo", {"test"})>p1).run();
    EXPECT_EQ(fs::exists(p1), true);


    shell("cat",{p1.string()})>s1;

    (shell("echo", {"append"})>>p1).run();

    EXPECT_EQ(ReadAllText3(p1), "test\nappend\n");

    shell("rm",{p1.string()});
    shell("rm",{p1.string()});
    EXPECT_EQ(fs::exists(p1), false);
}
