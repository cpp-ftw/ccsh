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
    mutable bool autorun_flag = true;
    void run_autorun() noexcept;
    friend class command_runnable;

public:
    virtual int runx(int, int, int) const = 0;
    int run() const;
    void no_autorun() const
    {
        autorun_flag = false;
    }
    virtual ~command_base() { }
};

class command_runnable : protected std::shared_ptr<command_base>
{
    using base = std::shared_ptr<command_base>;

public:

    using base::base;

    command_runnable(command_runnable const& other)
        : base(other)
    {
        (*this)->no_autorun();
    }
    command_runnable(command_runnable&& old)
        : base(std::move(old))
    {
        (*this)->no_autorun();
    }


    command_runnable& operator=(command_runnable const& other)
    {
        static_cast<base&>(*this) = static_cast<base const&>(other);
        (*this)->no_autorun();
        return *this;
    }

    command_runnable& operator=(command_runnable&& old)
    {
        static_cast<base&>(*this) = static_cast<base&&>(std::move(old));
        (*this)->no_autorun();
        return *this;
    }
    
    int run() const
    {
        return (*this)->run();
    }

    void no_autorun() const
    {
        (*this)->no_autorun();
    }

    int runx(int in, int out, int err) const
    {
        return (*this)->runx(in, out, err);
    }

    ~command_runnable()
    {
        if(*this)
            (*this)->run_autorun();
    }
};

class command
{
    command_runnable cmd;
public:
    command(command_runnable cmd)
        : cmd{cmd}
    {
        cmd.no_autorun();
    }

    int run() const
    {
        return cmd.run();
    }
};

class command_native : public command_base
{
    std::string str;
    std::vector<std::string> args;
    std::vector<const char*> argv;

public:

    command_native(std::string const& str, std::vector<std::string> const& args = {});
    int runx(int in, int out, int err) const override;
};

class command_pair : public command_base
{
protected:
    command_runnable left;
    command_runnable right;

public:
    command_pair(command_runnable left, command_runnable right)
        : left (left)
        , right(right)
    { }
};

class command_and final : public command_pair
{
public:
    using command_pair::command_pair;
    int runx(int in, int out, int err) const override
    {
        int lres = left.runx(in, out, err);
        if(lres != 0)
            return lres;
        return right.runx(in, out, err);
    }
};

class command_or final : public command_pair
{
public:
    using command_pair::command_pair;
    int runx(int in, int out, int err) const override
    {
        int lres = left.runx(in, out, err);
        if(lres == 0)
            return lres;
        return right.runx(in, out, err);
    }
};

class command_bool final : public command_base
{
    bool b;
public:
    command_bool(bool b)
        : b(b)
    { }

    int runx(int, int, int) const override
    {
        return !b; // logical inversion in bash
    }
};

class command_pipe final : public command_pair
{
public:
    using command_pair::command_pair;
    int runx(int in, int out, int err) const override;
};

class command_redirect : public command_base
{
protected:
    command_runnable c;
    open_wrapper fd;
public:
    command_redirect(command_runnable c, fs::path const& p, int flags);
};

class command_in_redirect final : public command_redirect
{
public:
    command_in_redirect(command_runnable c, fs::path const& p);

    int runx(int, int out, int err) const override
    {
        return c.runx(fd.get(), out, err);
    }
};

class command_out_redirect final : public command_redirect
{
public:
    command_out_redirect(command_runnable c, fs::path const& p, bool append = false);

    int runx(int in, int, int err) const override
    {
        return c.runx(in, fd.get(), err);
    }
};

class command_err_redirect final : public command_redirect
{
public:
    command_err_redirect(command_runnable c, fs::path const& p, bool append = false);

    int runx(int in, int out, int) const override
    {
        return c.runx(in, out, fd.get());
    }
};


} // namespace ccsh


#endif // CCSH_COMMAND_HPP_INCLUDED
