#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;

TEST(WrapperTest, cat)
{
    fs::path p1=ccsh::get_home()/"test.txt";

    shell("rm",{"-f", p1.string()});

    (shell("echo", {"test"})>p1).run();

    std::string natstr, wrapstr;

    shell("cat",{p1.string()})>natstr;

    cat(p1.string())>wrapstr;

    EXPECT_EQ(natstr, wrapstr);

    shell("rm",{p1.string()});
}
