#include "commons.hpp"

using namespace ccsh;

TEST(NativeTest, errorRedirection)
{
    fs::path p1=test_dir()/"surethereisnofilenamedlikethis.txt";

    std::string s1;
    shell("rm",{p1.string()})>=s1;

    EXPECT_NE(s1, "");
}
