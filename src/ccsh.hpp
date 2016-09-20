#ifndef CCSH_HPP_INCLUDED
#define CCSH_HPP_INCLUDED

#include "CWrapper.hpp"

#include <string>
#include <exception>
#include <memory>
#include <vector>

namespace ccsh
{

std::string pwd();

class stdc_error : public std::exception
{
    int error_number = errno;
public:
    virtual const char * what() const noexcept override;
};

inline void stdc_thrower(int result)
{
    if(result == -1)
        throw stdc_error();
}

struct open_traits
{
    static constexpr int invalid_value = -1;
    using exception = stdc_error;
    static void dtor_func(int fd) noexcept;
};

using open_wrapper = CW::CWrapper<int, open_traits>;

class command_base
{
public:
    virtual int runx(int, int) const = 0;
    int run() const;
    virtual ~command_base() { }
};

using command = std::shared_ptr<command_base>;

class command_native : public command_base
{
    std::string str;
    std::vector<std::string> args;
    std::vector<const char*> argv;

public:

    command_native(std::string const& str, std::vector<std::string> const& args = {});
    int runx(int in, int out) const override;
};


class command_pipe : public command_base
{
    command left;
    command right;

public:

    command_pipe(command left, command right)
        : left (left)
        , right(right)
    { }

    int runx(int in, int out) const override;
};

class command_out_redirect : public command_base
{
    command c;
    open_wrapper fd;

public:

    command_out_redirect(command c, std::string const& path);

    int runx(int in, int) const override
    {
        return c->runx(in, fd.get());
    }
};

inline command shell(std::string const& str, std::vector<std::string> const& args = {})
{
    return command{new command_native(str, args)};
}

inline command operator|(command a, command b)
{
    return command{new command_pipe(a, b)};
}

inline command operator>(command c, std::string const& path)
{
    return command{new command_out_redirect(c, path)};
}



} // namespace ccsh

#endif // CCSH_HPP_INCLUDED
