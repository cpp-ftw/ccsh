#ifndef CCSH_COMMAND_HPP_INCLUDED
#define CCSH_COMMAND_HPP_INCLUDED

#include "ccsh_utils.hpp"

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <functional>

namespace ccsh
{

using command_functor_raw  = std::function<ssize_t(char*, std::size_t)>;
using command_functor_init = std::function<void(void)>;

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

    friend class command;
    friend class command_pair;
    friend class command_redirect;
    friend class command_mapping;

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

public:

    command_runnable(command_base * other)
        : base(other)
    { }

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
    command(command_runnable const& cmd)
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
    command_pair(command_runnable const& left, command_runnable const& right)
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
        return !b; // logical inversion of shell logic
    }
};

class command_pipe final : public command_pair
{
public:
    using command_pair::command_pair;
    int runx(int in, int out, int err) const override;
};

class command_mapping : public command_base
{
protected:
    command_runnable c;
    command_functor_raw  func;
    command_functor_init init_func;
    
public:
    command_mapping(command_runnable const& c, command_functor_raw const& f, command_functor_init const& init_func = nullptr)
        : c(c)
        , func(f)
        , init_func(init_func)
    { }
};

class command_in_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    int runx(int, int, int) const override;
};

class command_out_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    int runx(int, int, int) const override;
};

class command_err_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    int runx(int, int, int) const override;
};


class command_redirect : public command_base
{
protected:
    command_runnable c;
    fs::path p;
    int flags;
    open_wrapper get_fd() const;
public:
    command_redirect(command_runnable const& c, fs::path const& p, int flags);
};

class command_in_redirect final : public command_redirect
{
public:
    command_in_redirect(command_runnable const& c, fs::path const& p);

    int runx(int, int out, int err) const override
    {
        return c.runx(get_fd().get(), out, err); // get_fd().get() is required for RAII
    }
};

class command_out_redirect final : public command_redirect
{
public:
    command_out_redirect(command_runnable const& c, fs::path const& p, bool append = false);

    int runx(int in, int, int err) const override
    {
        return c.runx(in, get_fd().get(), err);
    }
};

class command_err_redirect final : public command_redirect
{
public:
    command_err_redirect(command_runnable const& c, fs::path const& p, bool append = false);

    int runx(int in, int out, int) const override
    {
        return c.runx(in, out, get_fd().get());
    }
};


} // namespace ccsh


#endif // CCSH_COMMAND_HPP_INCLUDED
