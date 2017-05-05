#include "commons.hpp"

using namespace ccsh;

TEST(NativeTest, appendNotSameAsPipe)
{
    fs::path p1=ccsh::get_home()/"test.txt";

    shell("rm",{"-f", p1.string()});
    EXPECT_EQ(fs::exists(p1), false);

    (shell("echo", {"test"})>p1).run();
    EXPECT_EQ(fs::exists(p1), true);

    std::string s1;
    shell("cat",{p1.string()})>s1;

    (shell("echo", {"append"})>>p1).run();

    EXPECT_EQ(ReadAllText(p1), "test\nappend\n");

    (shell("echo", {"test"})>p1).run();
    EXPECT_EQ(ReadAllText(p1), "test\n");

    shell("rm",{p1.string()});
    EXPECT_EQ(fs::exists(p1), false);
}
