#include "commons.hpp"

#include <ccsh/core.hpp>

using namespace ccsh;

struct line_consumer
{
    std::string line;
    int repeat;
    mutable int actual = 0;

    line_consumer(std::string line, int repeat = 1)
        : line(std::move(line))
        , repeat(repeat)
    {}

    void operator()(std::string const& str) const
    {
        EXPECT_EQ(str, line);
        ++actual;
    }

    void check() const
    {
        EXPECT_EQ(repeat, actual);
    }
};

TEST(FunctionTest, line_functor_out)
{
    int repeat = 100;
    std::string str = "test";
    line_consumer consumer{str, repeat};

    std::string str_in;
    for(int i = 0; i < repeat; ++i)
    {
        str_in += str;
        str_in += '\n';
    }

    cat() < str_in > std::ref(consumer);

    EXPECT_EQ(str_in, "");
    consumer.check();
}

TEST(FunctionTest, line_functor_err)
{
    int repeat = 100;
    std::string str = "test";
    line_consumer consumer{str, repeat};

    auto cat_err = [&](fd_t, fd_t, fd_t err)
    {
        ofdstream output{err};
        for(int i = 0; i < repeat; ++i)
            output << str << std::endl;
        return 0;
    };

    command_make(cat_err) >= std::ref(consumer);
    consumer.check();
}
