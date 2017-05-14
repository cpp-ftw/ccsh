#include "commons.hpp"

using namespace ccsh;

TEST(PipeTest, simple)
{
    fs::path p1 = test_dir() / "test.txt"_p;

    shell("rm"_p, {"-f", p1.string()});
    EXPECT_EQ(fs::exists(p1), false);

    (shell("echo", {"test"}) > p1).run();
    EXPECT_EQ(fs::exists(p1), true);

    std::string s1;
    shell("cat"_p, {p1.string()}) > s1;
    trim(s1);

    std::string content = ReadAllText(p1);
    trim(content);

    EXPECT_EQ(s1, content);
    EXPECT_EQ(s1, "test");

    shell("rm"_p, {p1.string()});
    EXPECT_EQ(fs::exists(p1), false);
}



