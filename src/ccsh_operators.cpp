#include "ccsh_operators.hpp"

#include <cstring>
#include <functional>

using namespace std::placeholders;

namespace
{

ssize_t comh(std::string& str, char* buf, std::size_t s)
{
    str += std::string(buf, s);
    return s;
}

} // namespace

namespace ccsh 
{    

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
    return {new command_out_mapping(c, std::bind(comh, std::ref(str), _1, _2))};
}

command_runnable operator>(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    return {new command_out_mapping(c,  std::bind(comh, std::ref(str), _1, _2), 
                                        std::bind(&std::string::clear, std::ref(str)))};
}

command_runnable operator>>=(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    return {new command_err_mapping(c, std::bind(comh, std::ref(str), _1, _2))};
}

command_runnable operator>=(command_runnable const& c, std::string& str)
{
    c.no_autorun();
    return {new command_err_mapping(c,  std::bind(comh, std::ref(str), _1, _2), 
                                        std::bind(&std::string::clear, std::ref(str)))};
}

} // namespace ccsh
