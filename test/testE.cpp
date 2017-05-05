#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;

TEST(WrapperTest, inMapping)
{
    std::string s1, str_in="test";

    cat()<str_in>s1;

    EXPECT_EQ(str_in, "");
    EXPECT_EQ(s1, "test");
}
