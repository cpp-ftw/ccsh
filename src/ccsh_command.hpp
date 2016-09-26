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
using command_functor_line = std::function<void(std::string const&)>;

class command_builder_base
{
protected:
    virtual std::vector<std::string>& get_args() = 0;
    virtual std::vector<std::string> const& get_args() const = 0;
};

template<typename>
class command_holder;

class command_base
{
    mutable bool autorun_flag = true;
    void run_autorun() noexcept;

    friend class command_runnable;
    template<typename>
    friend class command_holder;

public:
    virtual int runx(int, int, int) const = 0;
    int run() const;
    void no_autorun() const
    {
        autorun_flag = false;
    }
    virtual ~command_base() { }
};

class command_native : public command_base
{
protected:

    std::string str;
    std::vector<std::string> args;

    template<typename>
    friend class command_holder;

    virtual std::vector<const char*> get_argv() const
    {
        std::vector<const char*> argv;
        argv.reserve(args.size() + 2);

        argv.push_back(this->str.c_str());
        for(const auto& s : this->args)
            argv.push_back(s.c_str());

        argv.push_back(nullptr);
        return argv;
    }

public:

    command_native(std::string const& str, std::vector<std::string> const& args = {})
        : str(str)
        , args(args)
    { }

    int runx(int in, int out, int err) const override final;
};

class command_runnable : protected std::shared_ptr<command_base>
{
    using base = std::shared_ptr<command_base>;

    friend class command;
    friend class command_pair;
    friend class command_redirect;
    friend class command_mapping;

    command_runnable(base b) :
        base(std::move(b))
    { }

    command_runnable(command_runnable const& other)
        : base(other)
    {
        no_autorun();
    }
    command_runnable(command_runnable&& old)
        : base(std::move(old))
    {
        no_autorun();
    }

    command_runnable& operator=(command_runnable const& other)
    {
        static_cast<base&>(*this) = static_cast<base const&>(other);
        no_autorun();
        return *this;
    }

    command_runnable& operator=(command_runnable&& old)
    {
        static_cast<base&>(*this) = static_cast<base&&>(std::move(old));
        no_autorun();
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
        if(*this)
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

template<typename TRAITS>
class command_holder : protected std::shared_ptr<command_native>, public TRAITS
{
    using base = std::shared_ptr<command_native>;

    friend class command;
    template<typename>
    friend class command_builder;
    friend class command_pair;
    friend class command_redirect;
    friend class command_mapping;

    command_holder(command_holder const& other)
        : base(other)
    {
        no_autorun();
    }
    command_holder(command_holder&& old)
        : base(std::move(old))
    {
        no_autorun();
    }

    command_holder& operator=(command_holder const& other)
    {
        static_cast<base&>(*this) = static_cast<base const&>(other);
        no_autorun();
        return *this;
    }

    command_holder& operator=(command_holder&& old)
    {
        static_cast<base&>(*this) = static_cast<base&&>(std::move(old));
        no_autorun();
        return *this;
    }

protected:

    virtual std::vector<std::string>& get_args() override final
    {
        return static_cast<command_native*>(get())->args;
    }

    virtual std::vector<std::string> const& get_args() const override final
    {
        return static_cast<command_native*>(get())->args;
    }


public:

    command_holder(command_base * other)
        : base(other)
    { }

    command_holder(std::string const& name, std::vector<std::string> const& args)
        : base(new command_native(name, args))
    { }

    int run() const
    {
        return (*this)->run();
    }

    void no_autorun() const
    {
        if(*this)
            (*this)->no_autorun();
    }

    int runx(int in, int out, int err) const
    {
        return (*this)->runx(in, out, err);
    }

    ~command_holder()
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

    template<typename T>
    command(command_holder<T> const& other)
        : cmd(std::dynamic_pointer_cast<command_base>(other))
    { }

    int run() const
    {
        return cmd.run();
    }
};

template<typename T>
class command_builder : public command_holder<T>
{
    using base = command_holder<T>;
public:
    command_builder(command_holder<T> const& cmd)
        : base{cmd}
    {
    }

};

/*template<typename T, typename... ARGS>
class command_builder<command_holder<T>(*)(ARGS...)>
    : public command_builder<T>
{
    using base = command_builder<T>;
public:
    using base::base;
};*/

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
