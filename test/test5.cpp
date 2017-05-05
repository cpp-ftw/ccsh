#include "commons.hpp"

using namespace ccsh;

TEST(WrapperTest, dollar)
{
    std::string str="test";

    dollar("VARNATIVE")=str;
    ccsh::$("VARWRAPPER")=str;

    std::string natstr=dollar("VARNATIVE");
    std::string wrapstr=$("VARWRAPPER");
    EXPECT_EQ(natstr, wrapstr);
}
