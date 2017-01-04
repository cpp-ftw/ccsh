#ifndef CCSH_UTILS_HPP_INCLUDED
#define CCSH_UTILS_HPP_INCLUDED

#include <exception>
#include <stdexcept>
#include <string>
#include <cstddef>

#include <boost/filesystem.hpp>

#include "CWrapper.hpp"

namespace ccsh {

namespace fs {
using namespace boost::filesystem;

std::vector<path> expand(path const& p);
std::vector<path> expand(std::vector<path> const& paths);
}
// NEVER EVER USE boost::filesystem DIRECTLY, ALWAYS USE ccsh::fs
// boost::filesystem WILL BE CHANGED TO std::filesystem WITH C++17

fs::path get_home();
fs::path get_current_path();
fs::path get_current_directory();
fs::path get_current_path_abbreviated();

std::string get_hostname();
std::string get_short_hostname();

std::string get_shell_name();

bool is_user_possibly_elevated();

class stdc_error : public std::runtime_error
{
    int error_number;
public:
    stdc_error(int no = errno)
        : runtime_error(strerror(errno))
        , error_number(no)
    {}

    stdc_error(int no, std::string const& msg)
        : std::runtime_error(msg.empty() ? strerror(no) : msg + ": " + strerror(no))
        , error_number(no)
    {}

    int no() const
    { return error_number; }
};


class shell_error : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};


class env_var
{
    std::string name;
public:
    env_var(std::string name)
        : name(std::move(name))
    {}

    env_var(env_var&&) = default;
    env_var(env_var const&) = delete;
    env_var& operator=(env_var&&) = default;
    env_var& operator=(env_var const&) = delete;

    operator std::string() const; // getenv
    env_var& operator=(std::string const&); // setenv
};

namespace internal {

struct open_traits
{
    static constexpr int invalid_value = -1;
    using exception = stdc_error;

    static void dtor_func(int fd) noexcept;
};

using open_wrapper = CW::CWrapper<int, open_traits>;

enum class stdfd : uint8_t
{
    in = 0,
    out = 1,
    err = 2,
    count
};


template<typename ENUM, std::size_t N>
const char* enum_to_string(ENUM val, const char* const (& mapping)[N])
{
    if(val < 0 || val >= N)
        return "";
    return mapping[val];
}

template<typename ENUM, std::size_t N>
const char* enum_to_string(ENUM val, std::array<const char*, N> const& mapping)
{
    if(val < 0 || val >= N)
        return "";
    return mapping[val];
}

} // namespace internal

} // namespace ccsh

#endif // CCSH_UTILS_HPP_INCLUDED
