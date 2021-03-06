#ifndef CCSH_CCSH_COMMAND_HPP
#define CCSH_CCSH_COMMAND_HPP

#include "ccsh_utils.hpp"

#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ccsh {
namespace internal {

using command_functor_raw  = std::function<int64_t(char*, std::size_t)>;
using command_functor_init = std::function<void(void)>;
using command_functor_line = std::function<void(std::string const&)>;
using command_functor      = std::function<int(fd_t, fd_t, fd_t)>;

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
    virtual void start_run(fd_t, fd_t, fd_t, std::vector<fd_t> unused_fds) const = 0;
    virtual int finish_run() const = 0;

    int run() const;

    void no_autorun() const
    {
        autorun_flag = false;
    }

    virtual ~command_base() = default;
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
    std::vector<tstring_t> args_;

    friend class command_source;

    virtual std::vector<const tchar_t*> get_argv() const
    {
        std::vector<const tchar_t*> argv;
        argv.reserve(args_.size() + 2);

        argv.push_back(p.c_str());
        for (const auto& s : this->args_)
            argv.push_back(s.c_str());

        argv.push_back(nullptr);
        return argv;
    }

public:
    void add_arg(const char* str);
    void add_arg(std::string const& str);
    void add_arg(std::string&& str);

#ifdef _WIN32
    void add_arg(const tchar_t* str);
    void add_arg(tstring_t const& str);
    void add_arg(tstring_t&& str);
#endif

    std::vector<tstring_t>& args() { return args_; }
    std::vector<tstring_t> const& args() const { return args_; }

    fs::path& binary() { return p; }
    fs::path const& binary() const { return p; }

    explicit command_native(fs::path p, std::vector<std::string> args = {});

    void append_dir(fs::path const& dir)
    {
        p = dir / p;
    }

    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const override;
    int finish_run() const override
    {
        return result.get();
    }

#ifdef _WIN32
private:
    void start_run_internal(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds, const tchar_t* appname, tstring_t cmdline) const;
    friend class command_source;
    friend class command_shell;
#endif
};

class command_runnable : protected std::shared_ptr<command_base>
{
    friend class command;

    command_runnable(command_runnable const&) = default;
    command_runnable(command_runnable&&) = default;
    command_runnable& operator=(command_runnable const&) = default;
    command_runnable& operator=(command_runnable&&) = default;

public:

    command_runnable(command_base* other)
        : std::shared_ptr<command_base>(other)
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

    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const
    {
        (*this)->start_run(in, out, err, std::move(unused_fds));
    }

    int finish_run() const
    {
        return (*this)->finish_run();
    }

    command_base* base() { return get(); }
    command_base const* base() const { return get(); }

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

    ~command_holder() override
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
    using command_runnable::base;
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
    command_pair(command left, command right)
        : left(std::move(left))
        , right(std::move(right))
    { }
};

class command_conditonal : public command_pair, protected command_async
{
public:
    using command_pair::command_pair;

    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const final
    {
        auto f = [=]
        {
            int lres = this->left.finish_run();
            if (!start_right(lres))
                return lres;
            this->right.start_run(in, out, err, unused_fds);
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
    explicit command_bool(bool b)
        : b(b)
    { }

    void start_run(fd_t, fd_t, fd_t, std::vector<fd_t>) const override
    { }

    int finish_run() const override
    {
        return int(!b); // logical inversion of shell logic
    }
};

class command_pipe final : public command_pair
{
public:
    using command_pair::command_pair;
    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const override;
    int finish_run() const override
    {
        int result1 = left.finish_run();
        int result2 = right.finish_run();
        return result1 == 0 ? result2 : result1;
    }
};

class command_mapping : public command_base, protected command_async
{
protected:
    command c;
    command_functor_raw func;
    command_functor_init init_func;

public:
    command_mapping(command c, command_functor_raw f, command_functor_init init_func = nullptr)
        : c(std::move(c))
        , func(std::move(f))
        , init_func(std::move(init_func))
    { }

    int finish_run() const final
    {
        result.wait();
        return c.finish_run();
    }
};

class command_in_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    void start_run(fd_t, fd_t, fd_t, std::vector<fd_t>) const override;
};

class command_out_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    void start_run(fd_t, fd_t, fd_t, std::vector<fd_t>) const override;
};

class command_err_mapping final : public command_mapping
{
public:
    using command_mapping::command_mapping;
    void start_run(fd_t, fd_t, fd_t, std::vector<fd_t>) const override;
};

template<stdfd DESC>
class command_redirect final : public command_base
{
    command c;
    fs::path p;
    int flags;
    mutable open_wrapper fd;
public:
    command_redirect(command c, fs::path p, bool append = false);
    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t>) const override;

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
    command_fd(command c, fd_t fd);
    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t>) const override;

    int finish_run() const override
    {
        return c.finish_run();
    }
};

class command_source final : public command_base, protected command_async
{
    mutable command_native cmd;
    tstring_t cmdstr;

public:
    explicit command_source(fs::path const& p, std::vector<std::string> const& args = {}, fs::path const& shell = "/bin/sh");

    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t>) const override;

    int finish_run() const override
    {
        result.wait();
        return cmd.finish_run();
    }
};

#ifdef _WIN32

class command_shell : public command_native
{
public:
    using command_native::command_native;
    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const override;
};

#endif // _WIN32

class command_builtin : public command_base
{
private:
    mutable int result = 0;
public:
    using command_base::command_base;

    virtual int runx(fd_t in, fd_t out, fd_t err) const = 0;

    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t>) const override
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
    explicit command_function(command_functor func)
        : func(std::move(func))
    { }

    void start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t>) const override;

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
    return internal::command_runnable{new internal::command_function{std::move(func)}};
}
} // namespace ccsh


#endif // CCSH_CCSH_COMMAND_HPP
