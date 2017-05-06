#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;

TEST(RedirectTest, append1)
{
    fs::path p1=test_dir()/"nofilenamedlikethis.txt";
    fs::path p2=test_dir()/"nofilenamedlikethis2.txt";

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
    shell("rm",{"-f", p2.string()});
    EXPECT_EQ(fs::exists(p1), false);
}

TEST(RedirectTest, append2)
{
    fs::path p1=test_dir()/"test.txt"_p;

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

TEST(RedirectTest, error_redirect)
{
    fs::path p1=test_dir()/"surethereisnofilenamedlikethis.txt";

    std::string s1;
    shell("rm",{p1.string()})>=s1;

    EXPECT_NE(s1, "");
}

TEST(RedirectTest, error_append)
{
    fs::path p1=test_dir()/"surethereisnofilenamedlikethis.txt"_p;

    std::string s1;
    shell("rm",{p1.string()})>=s1;
    std::string s2=s1;
    shell("rm",{p1.string()})>>=s1;


    //std::string reqstring="surethereisnocommandnamedlikethis: command not found\n";
    //reqstring.append(reqstring);
    EXPECT_NE(s1, s2);
}
