#include "ccsh_internals.hpp"
#include <ccsh/ccsh_operators.hpp>

#include <cstring>
#include <functional>

using namespace std::placeholders;

namespace ccsh {
namespace internal {

/* ******************* string redirection operators ******************* */

command_runnable operator<(command const& c, std::string& str)
{
    auto func = [&str](char* buf, std::size_t s) -> ssize_t
    {
        std::size_t len = str.length();
        len = len < s ? len : s;
        std::memcpy(buf, str.data(), len);
        if (len != 0)
            str.erase(0, len);
        return len;
    };
    return {new command_in_mapping(c, func)};
}

command_runnable operator>>(command const& c, std::string& str)
{
    return {new command_out_mapping(c, std::bind(mapping_appender, std::ref(str), _1, _2))};
}

command_runnable operator>(command const& c, std::string& str)
{
    return {new command_out_mapping(c, std::bind(mapping_appender, std::ref(str), _1, _2),
                                    std::bind(&std::string::clear, std::ref(str)))};
}

command_runnable operator>>=(command const& c, std::string& str)
{
    return {new command_err_mapping(c, std::bind(mapping_appender, std::ref(str), _1, _2))};
}

command_runnable operator>=(command const& c, std::string& str)
{
    return {new command_err_mapping(c, std::bind(mapping_appender, std::ref(str), _1, _2),
                                    std::bind(&std::string::clear, std::ref(str)))};
}

/* ******************* string redirection operators ******************* */


/* ******************* vector redirection operators ******************* */

command_runnable operator<(command const& c, std::vector<std::string>& vec)
{
    auto func = [&vec](char* buf, std::size_t s) -> ssize_t
    {
        if (s == 0 || vec.empty())
            return 0;
        std::string& str = vec[0];
        std::size_t len = str.length();
        if (len < s)
        {
            std::memcpy(buf, str.data(), len);
            buf[len] = '\n';
            vec.erase(vec.begin()); // shit
            return len + 1;
        }

        std::memcpy(buf, str.data(), s);
        str.erase(0, s);
        return s;
    };
    return {new command_in_mapping(c, func)};
}

command_runnable operator>(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str)
    { vec.push_back(std::move(str)); };
    return {new command_out_mapping(c, line_splitter_make(pusher),
                                    std::bind(&std::vector<std::string>::clear, std::ref(vec)))};
}

command_runnable operator>>(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str)
    { vec.push_back(std::move(str)); };
    return {new command_out_mapping(c, line_splitter_make(pusher))};
}

command_runnable operator>=(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str)
    { vec.push_back(std::move(str)); };
    return {new command_err_mapping(c, line_splitter_make(pusher),
                                    std::bind(&std::vector<std::string>::clear, std::ref(vec)))};
}

command_runnable operator>>=(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str)
    { vec.push_back(std::move(str)); };
    return {new command_err_mapping(c, line_splitter_make(pusher))};
}

/* ******************* vector redirection operators ******************* */


/* ******************* line functor redirection operators ******************* */

command_runnable operator>(command const& c, command_functor_line func)
{
    return {new command_out_mapping(c, line_splitter_make(std::move(func)))};
}

command_runnable operator>=(command const& c, command_functor_line func)
{
    return {new command_err_mapping(c, line_splitter_make(std::move(func)))};
}

/* ******************* line functor redirection operators ******************* */

/* ******************* raw functor redirection operators ******************* */

command_runnable operator<(command const& c, command_functor_raw func)
{
    return {new command_in_mapping(c, std::move(func))};
}

command_runnable operator>(command const& c, command_functor_raw func)
{
    return {new command_out_mapping(c, std::move(func))};
}

command_runnable operator>=(command const& c, command_functor_raw func)
{
    return {new command_err_mapping(c, std::move(func))};
}

/* ******************* raw functor redirection operators ******************* */

} // namespace internal
} // namespace ccsh
