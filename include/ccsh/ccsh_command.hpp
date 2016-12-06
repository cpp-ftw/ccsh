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
namespace internal
{

using command_functor_raw  = std::function<ssize_t(char*, std::size_t)>;
using command_functor_init = std::function<void(void)>;
using command_functor_line = std::function<void(std::string const&)>;

class command_builder_base
{
    template<typename>
    friend
    class command_holder;

    struct ccsh_abstract_guard_t
    {
    };
    virtual void ccsh_abstract_guard(ccsh_abstract_guard_t) const = 0;
};

template<typename>
class command_holder;

class command_base
{
    mutable bool autorun_flag = true;
    void run_autorun() noexcept;

    friend class command_runnable;

    template<typename>
    friend
    class command_holder;

public:
    virtual int runx(int, int, int) const = 0;
    int run() const;

    void no_autorun() const
    {
        autorun_flag = false;
    }

    virtual ~command_base()
    {}
};

class command_native : public command_base
{
protected:

    fs::path p;
    std::vector<std::string> args;

    virtual std::vector<const char*> get_argv() const
    {
        std::vector<const char*> argv;
        argv.reserve(args.size() + 2);

        argv.push_back(p.string().c_str());
        for(const auto& s : this->args)
            argv.push_back(s.c_str());

        argv.push_back(nullptr);
        return argv;
    }

public:

    command_native(fs::path const& p, std::vector<std::string> const& args = {})
        : p(p)
        , args(args)
    {}

    void append_dir(fs::path const& dir)
    {
        p = dir / p;
    }

    int runx(int in, int out, int err) const override final;
};

class command_runnable : protected std::shared_ptr<command_base>
{
    using base = std::shared_ptr<command_base>;

    friend class command;

    command_runnable(command_runnable const& other) = default;
    command_runnable(command_runnable&& old) = default;
    command_runnable& operator=(command_runnable const& other) = default;
    command_runnable& operator=(command_runnable&& old) = default;

public:

    command_runnable(command_base* other)
        : base(other)
    {}

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
class command_holder : public TRAITS
{
    friend class command;

    template<typename>
    friend
    class command_builder;

    command_holder(command_holder const& other) = default;
    command_holder(command_holder&& old) = default;
    command_holder& operator=(command_holder const& other) = default;
    command_holder& operator=(command_holder&& old) = default;

    void ccsh_abstract_guard(command_builder_base::ccsh_abstract_guard_t) const override
    {}

public:

    command_holder() = default;

    using TRAITS::TRAITS;

    ~command_holder()
    {
        TRAITS::run_autorun();
    }
};

class command : protected command_runnable
{
public:
    command(command_runnable const& cmd)
        : command_runnable{cmd}
    {
        cmd.no_autorun();
        command_runnable::no_autorun();
    }

    template<typename T>
    command(command_holder<T> const& cmd)
        : command_runnable{new command_holder<T>(cmd)}
    {
        cmd.no_autorun();
        command_runnable::no_autorun();
    }

    using command_runnable::run;

    // cannot "using" this because std::bind would stop working
    int runx(int in, int out, int err) const
    {
        return command_runnable::runx(in, out, err);
    }
};

template<typename T>
class command_builder : public command_holder<T>
{   // must use public inheritance to get all the methods of command_holder, which are mostly switches
    using base = command_holder<T>;
public:
    command_builder(command_holder<T> const& cmd)
        : base{cmd}
    {
        base::no_autorun();
        cmd.no_autorun();
    }
};

template<typename T>
class command_builder<command_holder<T>> : public command_builder<T>
{
    using base = command_builder<T>;
public:
    using base::base;
};

class command_pair : public command_base
{
protected:
    command left;
    command right;

public:
    command_pair(command const& left, command const& right)
        : left(left)
        , right(right)
    {}
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
    {}

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
    command c;
    command_functor_raw func;
    command_functor_init init_func;

public:
    command_mapping(command const& c, command_functor_raw const& f, command_functor_init const& init_func = nullptr)
        : c(c)
        , func(f)
        , init_func(init_func)
    {}
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

template<stdfd DESC>
class command_redirect final : public command_base
{
    command c;
    fs::path p;
    int flags;
public:
    command_redirect(command const& c, fs::path const& p, bool append = false);
    int runx(int in, int out, int err) const override;
};

template<stdfd DESC>
class command_fd final : public command_base
{
    command c;
    open_wrapper ow;
public:
    command_fd(command const& c, int fd);
    int runx(int in, int out, int err) const override;
};

class command_source final : public command_base
{
    fs::path p;
    std::vector<std::string> args;
public:
    command_source(fs::path const& p, std::vector<std::string> const& args = {})
        : p(p)
        , args(args)
    {}

    int runx(int in, int out, int err) const override;
};

} // namespace internal

using internal::command;
using internal::command_builder;
using internal::command_holder;  // easier for wrappers


} // namespace ccsh


#endif // CCSH_COMMAND_HPP_INCLUDED
