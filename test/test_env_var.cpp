#include "commons.hpp"

using namespace ccsh;

TEST(EnvVarTest, dollar1)
{
    std::string str = "test";

    dollar("TESTVAR") = str;

    std::string gotstr = dollar("TESTVAR");
    EXPECT_EQ(str, gotstr);
}

TEST(EnvVarTest, dollar2)
{
    std::string str = "test";

    dollar("VARNATIVE") = str;
    ccsh::$("VARWRAPPER") = str;

    std::string natstr = dollar("VARNATIVE");
    std::string wrapstr = $("VARWRAPPER");
    EXPECT_EQ(natstr, wrapstr);
}

TEST(EnvVarTest, source)
{
    fs::path vars = fs::path{__FILE__}.parent_path() / "vars.sh"_p;
    ASSERT_EQ(fs::exists(vars), true);
    source(vars);
    std::string var1, var2;
    var1 = $("EXAMPLE_ENV_VAR1");
    var2 = $("EXAMPLE_ENV_VAR2");
    EXPECT_EQ(var1, "");
    EXPECT_EQ(var2, "value2");
}
