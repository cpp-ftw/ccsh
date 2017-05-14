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
    auto func = [&str](char* buf, std::size_t s) -> int64_t
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
    auto func = [&vec](char* buf, std::size_t s) -> int64_t
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

/* ******************* stream redirection operators ******************* */

namespace {

template<typename COMMAND>
command_base * out_stream(command const& c, std::ostream& os)
{
    return new COMMAND(c, [&os](char* str, std::size_t n) -> int64_t {
        auto pos = os.tellp();
        if (os.write(str, n))
            return n;
        else
        {
            auto count = os.tellp() - pos;
            return count > 0 ? count : -1;
        }
    });
}

template<typename COMMAND>
command_base * in_stream(command const& c, std::istream& is)
{
    return new COMMAND(c, [&is](char* str, std::size_t n) -> int64_t {
        if (is.read(str, n))
            return n;
        else
        {
            auto count = is.gcount();
            return count > 0 ? count : -1;
        }
    });
}

}

command_runnable operator>>(command const& c, std::ostream& os)
{
    return {out_stream<command_out_mapping>(c, os)};
}
command_runnable operator<<(std::ostream& os, command const& c)
{
    return {out_stream<command_out_mapping>(c, os)};
}

command_runnable operator>>=(command const& c, std::ostream& os)
{
    return {out_stream<command_err_mapping>(c, os)};
}
command_runnable operator<<=(std::ostream& os, command const& c)
{
    return {out_stream<command_err_mapping>(c, os)};
}

command_runnable operator<<(command const& c, std::istream& is)
{
    return {in_stream<command_in_mapping>(c, is)};
}
command_runnable operator>>(std::istream& is, command const& c)
{
    return {in_stream<command_in_mapping>(c, is)};
}


/* ******************* stream redirection operators ******************* */

} // namespace internal
} // namespace ccsh
