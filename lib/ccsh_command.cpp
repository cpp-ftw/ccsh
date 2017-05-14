#include "ccsh_internals.hpp"
#include <ccsh/ccsh_command.hpp>

#include <ccsh/tmp-lib/call_kill.hpp>

#include <cstdio>
#include <cstring>

#include <iostream>
#include <utility>

namespace ccsh {
namespace internal {

using namespace std::placeholders;

int command_base::last_exit_code = 0;

void command_base::run_autorun() noexcept
{
    if (!autorun_flag)
        return;

    try
    {
        run();
    }
    catch (std::exception const& x)
    {
        std::cerr << x.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "An unhandled type of exception was thrown in command::autorun" << std::endl;
    }
}

void command_pipe::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> left_unused_fds) const
{
    std::vector<fd_t> right_unused_fds = left_unused_fds;
    pipe_t pipefd = pipe_compat();
    left_unused_fds.push_back(pipefd[0].get());
    right_unused_fds.push_back(pipefd[1].get());
    left.start_run(in, pipefd[1].get(), err, std::move(left_unused_fds));
    right.start_run(pipefd[0].get(), out, err, std::move(right_unused_fds));
}

void command_in_mapping::start_run(fd_t, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const
{
    if (init_func) init_func();

    pipe_t pipefd = pipe_compat();

    unused_fds.push_back(pipefd[1].get());

    auto f2 = [this](open_wrapper fd)
    {
        char buf[BUFSIZ];
        int64_t count;
        while ((count = func(buf, BUFSIZ)) > 0)
            write_compat(fd.get(), buf, std::size_t(count));

        return 0;
    };

    c.start_run(pipefd[0].get(), out, err, std::move(unused_fds));
    result = std::async(std::launch::async, f2, std::move(pipefd[1]));
}

void command_out_mapping::start_run(fd_t in, fd_t, fd_t err, std::vector<fd_t> unused_fds) const
{
    if (init_func) init_func();

    pipe_t pipefd = pipe_compat();

    unused_fds.push_back(pipefd[0].get());
    auto f2 = [this](open_wrapper fd)
    {
        char buf[BUFSIZ];
        std::size_t count;
        while ((count = read_compat(fd.get(), buf, BUFSIZ)) > 0)
            func(buf, count);

        stdc_thrower(count);

        return 0;
    };

    c.start_run(in, pipefd[1].get(), err, std::move(unused_fds));
    close_compat(pipefd[1].release());
    result = std::async(std::launch::async, f2, std::move(pipefd[0]));
}

void command_err_mapping::start_run(fd_t in, fd_t out, fd_t, std::vector<fd_t> unused_fds) const
{
    if (init_func) init_func();

    pipe_t pipefd = pipe_compat();

    unused_fds.push_back(pipefd[0].get());
    auto f2 = [this](open_wrapper fd)
    {
        char buf[BUFSIZ];
        std::size_t count;
        while ((count = read_compat(fd.get(), buf, BUFSIZ)) > 0)
            func(buf, count);

        stdc_thrower(count);

        return 0;
    };

    c.start_run(in, out, pipefd[1].get(), std::move(unused_fds));
    close_compat(pipefd[1].release());
    result = std::async(std::launch::async, f2, std::move(pipefd[0]));
}

template<stdfd DESC>
command_fd<DESC>::command_fd(command c, fd_t fd)
    : c(std::move(c))
    , ow(fd)
{ }

template<stdfd DESC>
void command_fd<DESC>::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const
{
    fd_t fds[int(stdfd::count)] = {in, out, err};
    fds[int(DESC)] = ow.get();
    c.start_run(fds[int(stdfd::in)], fds[int(stdfd::out)], fds[int(stdfd::err)], std::move(unused_fds));
}

template
class command_fd<stdfd::in>;

template
class command_fd<stdfd::out>;

template
class command_fd<stdfd::err>;


void command_function::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t>) const
{
    open_wrapper in2{duplicate_compat(in)};
    open_wrapper out2{duplicate_compat(out)};
    open_wrapper err2{duplicate_compat(err)};

    struct dup_helper // working around missing [=, temp0 = std::move(in2)] feature in C++11
    {
        mutable open_wrapper in2;
        mutable open_wrapper out2;
        mutable open_wrapper err2;
        command_function const* self;

        int operator()() const
        {
            return self->func(in2.get(), out2.get(), err2.get());
        }
    };

    dup_helper helper{std::move(in2), std::move(out2), std::move(err2), this};
    result = std::async(std::launch::async, tmp_lib::call_kill(std::move(helper)));
}

} // namespace internal
} // namespace ccsh

