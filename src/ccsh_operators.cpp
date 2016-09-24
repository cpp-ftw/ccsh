#include "ccsh_operators.hpp"

#include <cstring>
#include <functional>

using namespace std::placeholders;

namespace
{

ssize_t mapping_appender(std::string& str, char* buf, std::size_t s)
{
    str += std::string(buf, s);
    return s;
}

template<typename FUNC>
class line_splitter
{
    std::string temp;
    FUNC func;
public:
    explicit line_splitter(FUNC&& func)
        : func(std::move(func))
    { }

    ssize_t operator()(char* buf, std::size_t s)
    {   // possible to increase efficiency with tricky std::move or std::swap
        char * newline;
        size_t s_cpy = s;
        while (s > 0 && (newline = (char*)memchr(buf, '\n', s)))
        {
            size_t diff = newline-buf;
            temp += std::string(buf, diff);
            func(temp);
            temp.clear();
            buf += diff + 1;
            s -= diff + 1;
        }
        temp += std::string(buf, s);
        return s_cpy;
    }
};

template<typename FUNC>
line_splitter<FUNC> line_splitter_make(FUNC&& func)
{   // handy when you have a lambda
    return line_splitter<FUNC>(std::move(func));
}

} // namespace

namespace ccsh
{

/* ******************* string redirection operators ******************* */

command_runnable operator<(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    auto func = [&str](char* buf, std::size_t s) -> ssize_t
    {
        std::size_t len = str.length();
        len = len < s ? len : s;
        std::memcpy(buf, str.data(), len);
        if(len)
            str.erase(0, len);
        return len;
    };
    return {new command_in_mapping(c, func)};
}

command_runnable operator>>(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    return {new command_out_mapping(c, std::bind(mapping_appender, std::ref(str), _1, _2))};
}

command_runnable operator>(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    return {new command_out_mapping(c,  std::bind(mapping_appender, std::ref(str), _1, _2),
                                        std::bind(&std::string::clear, std::ref(str)))};
}

command_runnable operator>>=(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    return {new command_err_mapping(c, std::bind(mapping_appender, std::ref(str), _1, _2))};
}

command_runnable operator>=(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    return {new command_err_mapping(c,  std::bind(mapping_appender, std::ref(str), _1, _2),
                                        std::bind(&std::string::clear, std::ref(str)))};
}

/* ******************* string redirection operators ******************* */


/* ******************* vector redirection operators ******************* */

command_runnable operator<(command_runnable const& c, std::vector<std::string>& vec)
{
    c.no_autorun();
    auto func = [&vec](char* buf, std::size_t s) -> ssize_t
    {
        if(s == 0 || vec.empty())
            return 0;
        std::string& str = vec[0];
        std::size_t len = str.length();
        if(len < s)
        {
            std::memcpy(buf, str.data(), len);
            buf[len] = '\n';
            vec.erase(vec.begin()); // shit
            return len + 1;
        }
        else
        {
            std::memcpy(buf, str.data(), s);
            str.erase(0, s);
            return s;
        }
    };
    return {new command_in_mapping(c, func)};
}

command_runnable operator>(command_runnable const& c, std::vector<std::string>& vec)
{
    c.no_autorun();
    auto pusher = [&vec](std::string const& str) { vec.push_back(str); };
    return {new command_out_mapping(c,  line_splitter_make(std::move(pusher)),
                                        std::bind(&std::vector<std::string>::clear, std::ref(vec)))};
}
command_runnable operator>>(command_runnable const& c, std::vector<std::string>& vec)
{
    c.no_autorun();
    auto pusher = [&vec](std::string const& str) { vec.push_back(str); };
    return {new command_out_mapping(c,  line_splitter_make(std::move(pusher)))};
}

command_runnable operator>=(command_runnable const& c, std::vector<std::string>& vec)
{
    c.no_autorun();
    auto pusher = [&vec](std::string const& str) { vec.push_back(str); };
    return {new command_err_mapping(c,  line_splitter_make(std::move(pusher)),
                                        std::bind(&std::vector<std::string>::clear, std::ref(vec)))};
}
command_runnable operator>>=(command_runnable const& c, std::vector<std::string>& vec)
{
    c.no_autorun();
    auto pusher = [&vec](std::string const& str) { vec.push_back(str); };
    return {new command_err_mapping(c,  line_splitter_make(std::move(pusher)))};
}

/* ******************* vector redirection operators ******************* */


/* ******************* line functor redirection operators ******************* */

command_runnable operator>(command_runnable const& c, command_functor_line func)
{
    c.no_autorun();
    return {new command_out_mapping(c,  line_splitter_make(std::move(func)))};
}

command_runnable operator>=(command_runnable const& c, command_functor_line func)
{
    c.no_autorun();
    return {new command_err_mapping(c,  line_splitter_make(std::move(func)))};
}

/* ******************* line functor redirection operators ******************* */

/* ******************* raw functor redirection operators ******************* */

command_runnable operator<(command_runnable const& c, command_functor_raw func)
{
    c.no_autorun();
    return {new command_in_mapping(c,  std::move(func))};
}

command_runnable operator>(command_runnable const& c, command_functor_raw func)
{
    c.no_autorun();
    return {new command_out_mapping(c,  std::move(func))};
}

command_runnable operator>=(command_runnable const& c, command_functor_raw func)
{
    c.no_autorun();
    return {new command_err_mapping(c,  std::move(func))};
}

/* ******************* raw functor redirection operators ******************* */


} // namespace ccsh
