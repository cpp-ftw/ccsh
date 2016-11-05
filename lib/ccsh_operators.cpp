#include <ccsh/ccsh_operators.hpp>

#include <cstring>
#include <functional>

using namespace std::placeholders;

namespace
{

ssize_t mapping_appender(std::string& str, char* buf, std::size_t s)
{
    str.append(buf, s);
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
    {
        char * newline;
        size_t si = s;
        while (si > 0 && (newline = (char*)memchr(buf, '\n', si)))
        {
            size_t diff = newline-buf;
            temp.append(buf, diff);
            // if you want efficient processing, func should take std::string&& argument
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
line_splitter<FUNC> line_splitter_make(FUNC&& func)
{   // Comes handy when you have a lambda.
    // If you see an error here: *call this function only with rvalues*!
    // Cannot be done better without type_traits because of "forwarding reference".
    return line_splitter<FUNC>(std::forward<FUNC>(func));
}

} // namespace

namespace ccsh
{

/* ******************* string redirection operators ******************* */

command_runnable operator<(command const& c, std::string& str)
{
    auto func = [&str](char* buf, std::size_t s) -> ssize_t
    {
        std::size_t len = str.length();
        len = len < s ? len : s;
        std::memcpy(buf, str.data(), len);
        if(len)
            str.erase(0, len);
        return len;
    };
    return {new command_mapping<stdfd::in>(c, func)};
}

command_runnable operator>>(command const& c, std::string& str)
{
    return {new command_mapping<stdfd::out>(c, std::bind(mapping_appender, std::ref(str), _1, _2))};
}

command_runnable operator>(command const& c, std::string& str)
{
    return {new command_mapping<stdfd::out>(c,  std::bind(mapping_appender, std::ref(str), _1, _2),
                                                std::bind(&std::string::clear, std::ref(str)))};
}

command_runnable operator>>=(command const& c, std::string& str)
{
    return {new command_mapping<stdfd::err>(c, std::bind(mapping_appender, std::ref(str), _1, _2))};
}

command_runnable operator>=(command const& c, std::string& str)
{
    return {new command_mapping<stdfd::err>(c,  std::bind(mapping_appender, std::ref(str), _1, _2),
                                                std::bind(&std::string::clear, std::ref(str)))};
}

/* ******************* string redirection operators ******************* */


/* ******************* vector redirection operators ******************* */

command_runnable operator<(command const& c, std::vector<std::string>& vec)
{
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
    return {new command_mapping<stdfd::in>(c, func)};
}

command_runnable operator>(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str) { vec.push_back(std::move(str)); };
    return {new command_mapping<stdfd::out>(c,  line_splitter_make(std::move(pusher)),
                                                std::bind(&std::vector<std::string>::clear, std::ref(vec)))};
}
command_runnable operator>>(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str) { vec.push_back(std::move(str)); };
    return {new command_mapping<stdfd::out>(c,  line_splitter_make(std::move(pusher)))};
}

command_runnable operator>=(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str) { vec.push_back(std::move(str)); };
    return {new command_mapping<stdfd::err>(c,  line_splitter_make(std::move(pusher)),
                                                std::bind(&std::vector<std::string>::clear, std::ref(vec)))};
}
command_runnable operator>>=(command const& c, std::vector<std::string>& vec)
{
    auto pusher = [&vec](std::string&& str) { vec.push_back(std::move(str)); };
    return {new command_mapping<stdfd::err>(c,  line_splitter_make(std::move(pusher)))};
}

/* ******************* vector redirection operators ******************* */


/* ******************* line functor redirection operators ******************* */

command_runnable operator>(command const& c, command_functor_line func)
{
    return {new command_mapping<stdfd::out>(c, line_splitter_make(std::move(func)))};
}

command_runnable operator>=(command const& c, command_functor_line func)
{
    return {new command_mapping<stdfd::err>(c, line_splitter_make(std::move(func)))};
}

/* ******************* line functor redirection operators ******************* */

/* ******************* raw functor redirection operators ******************* */

command_runnable operator<(command const& c, command_functor_raw func)
{
    return {new command_mapping<stdfd::in>(c, std::move(func))};
}

command_runnable operator>(command const& c, command_functor_raw func)
{
    return {new command_mapping<stdfd::out>(c, std::move(func))};
}

command_runnable operator>=(command const& c, command_functor_raw func)
{
    return {new command_mapping<stdfd::err>(c, std::move(func))};
}

/* ******************* raw functor redirection operators ******************* */


} // namespace ccsh