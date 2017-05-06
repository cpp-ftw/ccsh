#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;

TEST(MappingTest, string_in_out)
{
    std::string s1, str_in = "test";

    cat() < str_in > s1;

    EXPECT_EQ(str_in, "");
    EXPECT_EQ(s1, "test");
}

TEST(MappingTest, vector_in_out)
{
    std::size_t no = 100;
    std::string line = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    std::vector<std::string> vec_in;
    std::generate_n(std::back_inserter(vec_in), no, [&]{ return line; });
    ASSERT_EQ(vec_in.size(), no);

    std::vector<std::string> vec_out;

    cat() < vec_in > vec_out;

    EXPECT_EQ(vec_in.size(), 0);
    EXPECT_EQ(vec_out.size(), no);
    for(auto const& elem : vec_out)
        EXPECT_EQ(elem, line);
}
