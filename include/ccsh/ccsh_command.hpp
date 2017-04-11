#ifndef CCSH_COMMAND_HPP_INCLUDED
#define CCSH_COMMAND_HPP_INCLUDED

#include "ccsh_utils.hpp"

#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include <functional>
#include <future>
#include <utility>

namespace ccsh {
namespace internal {

using command_functor_raw  = std::function<ssize_t(char*, std::size_t)>;
using command_functor_init = std::function<void(void)>;
using command_functor_line = std::function<void(std::string const&)>;
using command_functor      = std::function<int(int, int, int)>;

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

    static int last_exit_code;

    friend class command;

public:
    virtual void start_run(int, int, int, std::vector<int> unused_fds) const = 0;
    virtual int finish_run() const = 0;

    int run() const;

    void no_autorun() const
    {
        autorun_flag = false;
    }

    virtual ~command_base()
    { }
};

class command_async
{
protected:
    mutable std::future<int> result;

    command_async() = default;
    command_async(command_async&&) = default;

    command_async(command_async const&)
    { }

    command_async& operator=(command_async&& other) = default;

    command_async& operator=(command_async const&)
    {
        result = {};
        return *this;
    }
};

class command_native : public command_base, private command_async
{
protected:

    fs::path p;
    std::vector<std::string> args;

    friend class command_source;

    virtual std::vector<const char*> get_argv() const
    {
        std::vector<const char*> argv;
        argv.reserve(args.size() + 2);

        argv.push_back(p.c_str());
        for (const auto& s : this->args)
            argv.push_back(s.c_str());

        argv.push_back(nullptr);
        return argv;
    }

public:

    command_native(fs::path const& p, std::vector<std::string> const& args = {})
        : p(p)
        , args(args)
    { }

    void append_dir(fs::path const& dir)
    {
        p = dir / p;
    }

    void start_run(int in, int out, int err, std::vector<int> unused_fds) const override final;
    virtual int finish_run() const override final;
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
    { }

    int run() const
    {
        return (*this)->run();
    }

    void no_autorun() const
    {
        if (*this)
            (*this)->no_autorun();
    }

    void start_run(int in, int out, int err, std::vector<int> unused_fds) const
    {
        (*this)->start_run(in, out, err, std::move(unused_fds));
    }

    int finish_run() const
    {
        return (*this)->finish_run();
    }

    ~command_runnable()
    {
        if (*this)
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
    static int last_exit_code()
    {
        return command_base::last_exit_code;
    }

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
    using command_runnable::start_run;
    using command_runnable::finish_run;
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
    { }
};

class command_conditonal : public command_pair, protected command_async
{
public:
    using command_pair::command_pair;

    void start_run(int in, int out, int err, std::vector<int> unused_fds) const override final
    {
        auto f = [=]
        {
            int lres = this->left.finish_run();
            if (!start_right(lres))
                return lres;
            this->right.start_run(in, out, err, std::move(unused_fds));
            return this->right.finish_run();
        };

        left.start_run(in, out, err, std::move(unused_fds));
        result = std::async(std::launch::async, f);
    }

    int finish_run() const override
    {
        return result.get();
    }

    virtual bool start_right(int left_result) const = 0;
};

class command_and final : public command_conditonal
{
public:
    using command_conditonal::command_conditonal;

    bool start_right(int left_result) const override
    {
        return left_result == 0;
    }
};

class command_or final : public command_conditonal
{
public:
    using command_conditonal::command_conditonal;

    bool start_right(int left_result) const override
    {
        return left_result != 0;
    }
};

class command_comma final : public command_conditonal
{
public:
    using command_conditonal::command_conditonal;

    bool start_right(int) const override
    {
        return true;
    }
};

class command_bool final : public command_base
{
    bool b;
public:
    command_bool(bool b)
        : b(b)
    { }

    void start_run(int, int, int, std::vector<int>) const override
    { }

    int finish_run() const override
    {
        return !b; // logical inversion of shell logic
    }
};

class command_pipe final : public command_pair
{
public:
    using command_pair::command_pair;
    void start_run(int in, int out, int err, std::vector<int> unused_fds) const override;
    int finish_run() const override;
};

class command_mapping : public command_base, protected command_async
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
    { }

    int finish_run() const override final
    {
        result.wait();
        return c.finish_run();
    }
};

class command_in_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    void start_run(int, int, int, std::vector<int>) const override;
};

class command_out_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    void start_run(int, int, int, std::vector<int>) const override;
};

class command_err_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    void start_run(int, int, int, std::vector<int>) const override;
};

template<stdfd DESC>
class command_redirect final : public command_base
{
    command c;
    fs::path p;
    int flags;
    mutable open_wrapper fd;
public:
    command_redirect(command const& c, fs::path const& p, bool append = false);
    void start_run(int in, int out, int err, std::vector<int>) const override;

    int finish_run() const override
    {
        int result = 0;
        try
        {
            result = c.finish_run();
            fd = open_wrapper{};
        }
        catch (...)
        {
            fd = open_wrapper{};
            throw;
        }
        return result;
    }
};

template<stdfd DESC>
class command_fd final : public command_base
{
    command c;
    open_wrapper ow;
public:
    command_fd(command const& c, int fd);
    void start_run(int in, int out, int err, std::vector<int>) const override;

    int finish_run() const override
    {
        return c.finish_run();
    }
};

class command_source final : public command_base, protected command_async
{
    mutable command_native cmd;
    std::string cmdstr;

public:
    command_source(fs::path const& p, std::vector<std::string> const& args = {});

    void start_run(int in, int out, int err, std::vector<int>) const override;

    int finish_run() const override
    {
        result.wait();
        return cmd.finish_run();
    }
};

class command_builtin : public command_base
{
private:
    mutable int result;
public:
    using command_base::command_base;

    virtual int runx(int in, int out, int err) const = 0;

    void start_run(int in, int out, int err, std::vector<int>) const override
    {
        result = runx(in, out, err);
    }

    int finish_run() const override
    {
        return result;
    }
};

class command_function : public command_base, protected command_async
{
    command_functor func;
public:
    command_function(command_functor func)
        : func(std::move(func))
    { }

    void start_run(int in, int out, int err, std::vector<int>) const override;

    int finish_run() const override
    {
        return result.get();
    }
};
} // namespace internal

using internal::command;
using internal::command_builder;
using internal::command_holder;  // easier for wrappers

inline command command_make(internal::command_functor func)
{
    return {new internal::command_function{std::move(func)}};
}
} // namespace ccsh


#endif // CCSH_COMMAND_HPP_INCLUDED
