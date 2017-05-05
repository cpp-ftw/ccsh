#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;

TEST(WrapperTest, ls_wc_arg)
{
    std::string natstr, wrapstr, natwrapstr;

    (shell("ls",{"-l"})|shell("wc", {"-l"}))>natstr;

    (ls().l()|wc().l())>wrapstr;

    EXPECT_EQ(natstr, wrapstr);
}
