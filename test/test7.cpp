#include <ccsh/ccsh.hpp>
#include <cstdio>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh;

std::string ReadAllText4(fs::path const& filename)
{
    std::ifstream t(filename.c_str());
    return std::string(std::istreambuf_iterator<char>(t),
                       std::istreambuf_iterator<char>());
}

TEST(NativeTest, appendNotSameAsPipe)
{
    fs::path p1=ccsh::get_home()/"test.txt";

    shell("rm",{p1.string()});
    EXPECT_EQ(fs::exists(p1), false);

    (shell("echo", {"test"})>p1).run();
    EXPECT_EQ(fs::exists(p1), true);

    std::string s1;
    shell("cat",{p1.string()})>s1;

    (shell("echo", {"append"})>>p1).run();

    EXPECT_EQ(ReadAllText4(p1), "test\nappend\n");

    (shell("echo", {"test"})>p1).run();
    EXPECT_EQ(ReadAllText4(p1), "test\n");

    shell("rm",{p1.string()});
    EXPECT_EQ(fs::exists(p1), false);
}
