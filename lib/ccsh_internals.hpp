#ifndef CCSH_INTERNALS_HPP_INCLUDED
#define CCSH_INTERNALS_HPP_INCLUDED

#include <string>
#include <cstddef>
#include <unistd.h>

namespace ccsh
{

inline void stdc_thrower(int result)
{
    if(result == -1)
        throw stdc_error();
}

inline int shell_logic_or(int a, int b)
{
    return a == 0 ? b : a;
}

inline void close_fd(int fd) noexcept
{
    while(close(fd) == -1 && errno == EINTR);
}

inline ssize_t mapping_appender(std::string& str, char* buf, std::size_t s)
{
    str.append(buf, s);
    return s;
}

template<typename FUNC>
class line_splitter
{
    std::string temp;
    FUNC func;
    char delim;
public:
    explicit line_splitter(FUNC&& func, char delim = '\n')
            : func(std::move(func))
              , delim(delim)
    {}

    ssize_t operator()(char* buf, std::size_t s)
    {
        char* newline;
        std::size_t si = s;
        while(si > 0 && (newline = (char*)memchr(buf, delim, si)))
        {
            std::size_t diff = newline - buf;
            temp.append(buf, diff);
            // if you want efficient processing, func can take std::string&& argument
            func(std::move(temp));
            temp.clear();
            buf += diff + 1;
            si -= diff + 1;
        }
        temp.append(buf, si);
        return s;
    }
};

template<typename FUNC>
line_splitter<FUNC> line_splitter_make(FUNC&& func, char delim = '\n')
{   // Comes handy when you have a lambda.
    // If you see an error here: *call this function only with rvalues*!
    // Cannot be done better without type_traits because of "forwarding reference".
    return line_splitter<FUNC>(std::forward<FUNC>(func), delim);
}

} // namespace ccsh

#endif // CCSH_INTERNALS_HPP_INCLUDED
