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
    a->no_autorun();
    b->no_autorun();
    return command{new command_pipe(a, b)};
}

inline command operator<(command c, fs::path const& p)
{
    c->no_autorun();
    return command{new command_in_redirect(c, p)};
}

inline command operator>(command c, fs::path const& p)
{
    c->no_autorun();
    return command{new command_out_redirect(c, p, false)};
}

inline command operator>>(command c, fs::path const& p)
{
    c->no_autorun();
    return command{new command_out_redirect(c, p, true)};
}

inline command operator>=(command c, fs::path const& p)
{
    c->no_autorun();
    return command{new command_err_redirect(c, p, false)};
}

inline command operator>>=(command c, fs::path const& p)
{
    c->no_autorun();
    return command{new command_err_redirect(c, p, true)};
}

inline command operator&&(command a, command b)
{
    a->no_autorun();
    b->no_autorun();
    return command{new command_and(a, b)};
}

inline command operator||(command a, command b)
{
    a->no_autorun();
    b->no_autorun();
    return command{new command_or(a, b)};
}

inline command operator&&(command a, bool b)
{
    a->no_autorun();
    return command{new command_and(a, command{new command_bool(b)})};
}

inline command operator||(command a, bool b)
{
    a->no_autorun();
    return command{new command_or(a, command{new command_bool(b)})};
}

inline command operator&&(bool b, command a) // provided only for consistency, useless anyways
{
    a->no_autorun();
    return command{new command_and(command{new command_bool(b)}, a)};
}

inline command operator||(bool b, command a) // provided only for consistency, useless anyways
{
    a->no_autorun();
    return command{new command_or(command{new command_bool(b)}, a)};
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
