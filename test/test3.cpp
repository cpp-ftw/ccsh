#include "commons.hpp"

using namespace ccsh;

TEST(NativeTest, pipe)
{
    fs::path p1=test_dir()/"test.txt"_p;

    shell("rm",{"-f", p1.string()});
    EXPECT_EQ(fs::exists(p1), false);

    (shell("echo", {"test"})>p1).run();
    EXPECT_EQ(fs::exists(p1), true);

    std::string s1;
    shell("cat",{p1.string()})>s1;

    EXPECT_EQ(s1, ReadAllText(p1));
    EXPECT_EQ(s1, "test\n");

    shell("rm",{p1.string()});
    EXPECT_EQ(fs::exists(p1), false);
}
