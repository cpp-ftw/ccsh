#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;
using namespace std::placeholders;

TEST(MappingTest, string_in_out)
{
    std::string test_data = "test";
    std::string s1, str_in = test_data;

    cat() < str_in > s1;

    EXPECT_EQ(str_in, "");
    EXPECT_EQ(s1, test_data);

    str_in = test_data;
    (cat() < str_in) >> s1;
    EXPECT_EQ(str_in, "");
    EXPECT_EQ(s1, test_data + test_data);
}

class MappingVectorTest : public ::testing::Test
{
protected:

    int no = 100;
    std::string line = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    std::vector<std::string> vec_in;
    command cat_err = command_make(std::bind(&MappingVectorTest::cat_err_func, this, _1, _2, _3));

    int cat_err_func(fd_t, fd_t, fd_t err)
    {
        ofdstream output{err};
        for(int i = 0; i < no; ++i)
            output << line << std::endl;
        return 0;
    }

    void SetUp()
    {
        std::generate_n(std::back_inserter(vec_in), no, [&]{ return line; });
        ASSERT_EQ(vec_in.size(), no);
    }
};

TEST_F(MappingVectorTest, vector_in_out)
{
    std::vector<std::string> vec_out = {"Content to be erased."};
    cat() < vec_in > vec_out;

    EXPECT_EQ(vec_in.size(), 0);
    EXPECT_EQ(vec_out.size(), no);
    for(auto const& elem : vec_out)
        EXPECT_EQ(elem, line);
}

TEST_F(MappingVectorTest, vector_out_append)
{
    std::vector<std::string> vec_out = {line};
    (cat() < vec_in) >> vec_out;

    EXPECT_EQ(vec_in.size(), 0);
    EXPECT_EQ(vec_out.size(), no + 1);
    for(auto const& elem : vec_out)
        EXPECT_EQ(elem, line);
}

TEST_F(MappingVectorTest, vector_err)
{
    std::vector<std::string> vec_out = {"Content to be erased."};
    cat_err >= vec_out;

    EXPECT_EQ(vec_out.size(), no);
    for(auto const& elem : vec_out)
        EXPECT_EQ(elem, line);
}

TEST_F(MappingVectorTest, vector_err_append)
{
    std::vector<std::string> vec_out = {line};
    cat_err >>= vec_out;

    EXPECT_EQ(vec_out.size(), no + 1);
    for(auto const& elem : vec_out)
        EXPECT_EQ(elem, line);
}
