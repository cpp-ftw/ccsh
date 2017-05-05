#define CCSH_NON_STANDARD_DOLLAR

#include "commons.hpp"

using namespace ccsh;

TEST(NativeTest, source)
{
    fs::path vars = fs::path{__FILE__}.parent_path() / "vars.sh"_p;
    ASSERT_EQ(fs::exists(vars), true);
    source(vars);
    std::string var1, var2;
    var1=$("EXAMPLE_ENV_VAR1");
    var2=$("EXAMPLE_ENV_VAR2");
    EXPECT_EQ(var1, "");
    EXPECT_EQ(var2, "value2");
}
