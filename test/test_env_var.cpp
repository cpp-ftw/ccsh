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
#ifdef _WIN32
    fs::path script = "var test.bat"_p;
#else
    fs::path script = "vars.sh"_p;
#endif

    fs::path vars = fs::path{__FILE__}.parent_path() / script;
    ASSERT_EQ(fs::exists(vars), true);

    std::string output;
    source(vars, {"value x"}, "/bin/bash") > output;
    rtrim(output);
    EXPECT_EQ(output, "\"value x\"");

    std::string var1, var2;
    var1 = $("EXAMPLE_ENV_VAR1");
    var2 = $("EXAMPLE_ENV_VAR2");
    EXPECT_EQ(var1, "");
    EXPECT_EQ(var2, "value2");
}

TEST(EnvVarTest, set)
{
    int result;

    result = env_var::try_set("NAME", "value");
    EXPECT_EQ(result, 0);

    result = env_var::try_set("", "value");
    EXPECT_LT(result, 0);

    EXPECT_THROW({
                     env_var::set("", "value");
                 }, stdc_error);

    EXPECT_NO_THROW({
                        env_var::set("NAME", "value");
                    });
}
