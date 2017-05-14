#ifndef CCSH_INTERNALS_HPP_INCLUDED
#define CCSH_INTERNALS_HPP_INCLUDED

#include "ccsh_internals_posix.hpp"
#include "ccsh_internals_windows.hpp"

#include <ccsh/ccsh_utils.hpp>

#include <string>
#include <type_traits>
#include <cstring>
#include <cstddef>
#include <cstdint>

namespace ccsh {
namespace internal {

inline void stdc_thrower(std::intmax_t result)
{
    if (result == -1)
        throw stdc_error();
}

inline void stdc_thrower(std::intmax_t result, std::string const& msg)
{
    if (result == -1)
        throw stdc_error(errno, msg);
}

inline int64_t mapping_appender(std::string& str, char* buf, std::size_t s)
{
    str.append(buf, s);
    return s;
}


template<typename FUNC>
void tokenize_string(std::string const& str, std::string const& delimiters, FUNC&& func)
{
    std::string line = str;
    char* saveptr = nullptr;
    char* token = strtok_r(&line[0], delimiters.c_str(), &saveptr);

    while (token != nullptr)
    {
        func(token);
        token = strtok_r(nullptr, delimiters.c_str(), &saveptr);
    }
}

} // namespace internal
} // namespace ccsh


#endif // CCSH_INTERNALS_HPP_INCLUDED