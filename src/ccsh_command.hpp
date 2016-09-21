#ifndef CCSH_COMMAND_HPP_INCLUDED
#define CCSH_COMMAND_HPP_INCLUDED

#include "ccsh_utils.hpp"

#include <string>
#include <memory>
#include <vector>

namespace ccsh
{

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

    command_out_redirect(command c, fs::path const& p);

    int runx(int in, int) const override
    {
        return c->runx(in, fd.get());
    }
};


} // namespace ccsh


#endif // CCSH_COMMAND_HPP_INCLUDED
