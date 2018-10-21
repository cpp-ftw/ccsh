#include "commons.hpp"

using namespace ccsh;

class BuiltinTest : public ::testing::Test
{
protected:
    fs::path wd;
    void SetUp()
    {
        wd = fs::current_path();
    }
    void TearDown()
    {
        cd{wd};
    }
};

TEST_F(BuiltinTest, pwd_backtick)
{
    fs::path pwd_fs = fs::current_path();
    fs::path pwd_ccsh = get_current_path();
    std::string pwd_native = $(shell("pwd"));
    pwd_native.pop_back(); // remove trailing '\n'
    std::string pwd_builtin = $(pwd());
    pwd_builtin.pop_back(); // remove trailing '\n'

    // they must be same as strings, not only refer to the same path!
    EXPECT_EQ(pwd_fs.string(), pwd_ccsh.string());
    EXPECT_EQ(pwd_fs.string(), pwd_native);
    EXPECT_EQ(pwd_fs.string(), pwd_builtin);
}

TEST_F(BuiltinTest, cd_relative)
{
    cd(".."_p);
    fs::path wd_after = fs::current_path();
    EXPECT_EQ(wd.parent_path(), wd_after);
}

TEST_F(BuiltinTest, cd_absolute)
{
    cd("/tmp"_p);
    fs::path wd_after = fs::current_path();
    EXPECT_EQ("/tmp"_p, wd_after);
}

TEST_F(BuiltinTest, fs_abbreviated)
{
    fs::path home = get_home();
    cd("/tmp"_p);
    fs::path wd = get_current_path_abbreviated();
    EXPECT_EQ(wd.string().find(home.string()), std::string::npos);
}

TEST_F(BuiltinTest, home_test)
{
    fs::path home = get_home();
    fs::path wd = get_current_path_abbreviated();
    EXPECT_EQ(wd.string().find(home.string()), std::string::npos);

    cd{get_home()};
    fs::path home_abbr = get_current_path_abbreviated();
    EXPECT_EQ(home_abbr.string(), "~");
}


