#include "commons.hpp"

using namespace ccsh;

TEST(NativeTest, CopyRemoveFile)
{
    fs::path example_path1{__FILE__};
    fs::path example_path2 = example_path1.parent_path() / "example1.file"_p;

    std::string example_content1 = ReadAllText(example_path1);
    EXPECT_NE(example_content1, "");

    shell("rm"_p, {"-f", example_path2.string()});
    EXPECT_EQ(fs::exists(example_path2), false);

    shell("cp"_p, {example_path1.string(), example_path2.string()});
    EXPECT_EQ(fs::exists(example_path2), true);

    std::string example_content2 = ReadAllText(example_path2);
    EXPECT_EQ(example_content1, example_content2);

    shell("rm"_p, {example_path2.string()});
    EXPECT_EQ(fs::exists(example_path2), false);
}

#ifdef _WIN32
TEST(NativeTest, ShellExecute)
{
    fs::path example_path1{__FILE__};
    fs::path example_path2 = example_path1.parent_path() / "var test.bat"_p;

    std::string output;
    cmd("dir", {"/B", example_path2.string()}) > output;
    ASSERT_EQ(output, "var test.bat" + portable_eol);
}
#endif
