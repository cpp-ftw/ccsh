#ifndef CCSH_UTILS_HPP_INCLUDED
#define CCSH_UTILS_HPP_INCLUDED

#include <exception>
#include <stdexcept>
#include <string>
#include <cstddef>

#include <boost/filesystem.hpp>

#include "CWrapper.hpp"

namespace ccsh
{

namespace fs = boost::filesystem;
// NEVER EVER USE boost::filesystem DIRECTLY, ALWAYS USE ccsh::fs
// boost::filesystem WILL BE CHANGED TO std::filesystem WITH C++17

fs::path get_home();

class stdc_error : public std::runtime_error
{
    int error_number;
public:
    stdc_error(int no = errno)
        : runtime_error(strerror(errno))
        , error_number(no)
    { }
    stdc_error(int no, std::string const& msg)
        : std::runtime_error(msg.empty() ? strerror(no) : msg + ": " + strerror(no))
        , error_number(no)
    { }
    int no() const { return error_number; }
};

inline void stdc_thrower(int result)
{
    if(result == -1)
        throw stdc_error();
}

class shell_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

inline void shell_thrower(int result, std::string const& str)
{
    if(result != 0)
        throw shell_error(str);
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

void close_fd(int fd) noexcept;

class env_var
{
    std::string name;
public:
    env_var(std::string name)
        : name(std::move(name))
    { }
    env_var(env_var&&) = default;
    env_var(env_var const&) = delete;
    env_var& operator=(env_var&&) = default;
    env_var& operator=(env_var const&) = delete;

    operator std::string() const; // getenv
    env_var& operator=(std::string const&); // setenv
};

template<typename ENUM, std::size_t N>
const char* enum_to_string(ENUM val, const char* const (&mapping)[N])
{
    if(val < 0 || val >= N)
        return "";
    return mapping[val];
}

} // namespace ccsh

#endif // CCSH_UTILS_HPP_INCLUDED
