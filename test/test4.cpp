#include "commons.hpp"

using namespace ccsh;

TEST(NativeTest, dollar)
{
    std::string str="test";

    dollar("TESTVAR")=str;

    std::string gotstr=dollar("TESTVAR");
    EXPECT_EQ(str, gotstr);
}
