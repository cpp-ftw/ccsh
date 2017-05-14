#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;


const std::string TEST_STRING = "Lorem ipsum\ndolor sit amit";

TEST(StreamTest, ostream)
{
    std::string input = TEST_STRING;
    std::ostringstream oss;
    (cat() < input) >> oss;
    EXPECT_EQ(input, "");
    EXPECT_EQ(oss.str(), TEST_STRING);
}

TEST(StreamTest, istream)
{
    std::string output;
    std::istringstream iss{TEST_STRING};
    (cat() > output) << iss;
    EXPECT_EQ(output, TEST_STRING);
    EXPECT_EQ(int(iss.tellg()), -1);
}

