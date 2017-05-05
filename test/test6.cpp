#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;

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

    EXPECT_EQ(ReadAllText(p1), "test\nappend\n");

    shell("rm",{"-f", p1.string()});
    shell("rm",{"-f", p1.string()});
    EXPECT_EQ(fs::exists(p1), false);
}
