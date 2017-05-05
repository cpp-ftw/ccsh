#include "commons.hpp"

using namespace ccsh;

TEST(NativeTest, errorAppend)
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
