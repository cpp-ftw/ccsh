#ifndef CCSH_OPERATORS_HPP_INCLUDED
#define CCSH_OPERATORS_HPP_INCLUDED

#include "ccsh_command.hpp"

namespace ccsh
{

inline command shell(std::string const& str, std::vector<std::string> const& args = {})
{
    return command{new command_native(str, args)};
}

inline command operator|(command a, command b)
{
    return command{new command_pipe(a, b)};
}

inline command operator>(command c, fs::path const& p)
{
    return command{new command_out_redirect(c, p)};
}


namespace literals
{

    inline fs::path operator""_p(const char* str, std::size_t)
    {
        return fs::path{str};
    }

} // namespace literals


} // namespace ccsh


#endif // CCSH_OPERATORS_HPP_INCLUDED
