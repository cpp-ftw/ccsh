#ifndef CCSH_UTILS_HPP_INCLUDED
#define CCSH_UTILS_HPP_INCLUDED

#include <exception>
#include <string>

#include <boost/filesystem.hpp>

#include "CWrapper.hpp"

namespace ccsh
{

namespace fs = boost::filesystem;
// NEVER EVER USE boost::filesystem DIRECTLY, ALWAYS USE ccsh::ffs
// boost::filesystem WILL BE CHANGED TO std::filesystem WITH C++17

std::string pwd();

class stdc_error : public std::exception
{
    int error_number = errno;
public:
    virtual const char * what() const noexcept override;
};

inline void stdc_thrower(int result)
{
    if(result == -1)
        throw stdc_error();
}

struct open_traits
{
    static constexpr int invalid_value = -1;
    using exception = stdc_error;
    static void dtor_func(int fd) noexcept;
};

using open_wrapper = CW::CWrapper<int, open_traits>;

inline int shell_logic_or(int a, int b)
{
    return a == 0 ? b : a;
}


} // namespace ccsh

#endif // CCSH_UTILS_HPP_INCLUDED