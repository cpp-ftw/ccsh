#include "ccsh_internals.hpp"
#include <ccsh/ccsh_command.hpp>

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <utility>

namespace ccsh {
namespace internal {

using namespace std::placeholders;

int command_base::last_exit_code = 0;

int command_base::run() const
{
    no_autorun();
    start_run(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, std::vector<int>());
    int result = finish_run();
    last_exit_code = result;
    return result;
}

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


void command_native::start_run(int in, int out, int err, std::vector<int> unused_fds) const
{
    int fail_pipe[2];
    stdc_thrower(pipe(fail_pipe));

    auto argv = get_argv();

    pid_t pid = fork();
    stdc_thrower(pid);

    if (pid == 0)
    {
        close_fd(fail_pipe[0]);

        for (int fd : unused_fds)
            close_fd(fd);

        if (in != STDIN_FILENO)
            if (CCSH_RETRY_HANDLER(dup2(in, STDIN_FILENO)) < 0)
                goto fail;

        if (out != STDOUT_FILENO)
            if (CCSH_RETRY_HANDLER(dup2(out, STDOUT_FILENO)) < 0)
                goto fail;

        if (err != STDERR_FILENO)
            if (CCSH_RETRY_HANDLER(dup2(err, STDERR_FILENO)) < 0)
                goto fail;

        if (CCSH_RETRY_HANDLER(fcntl(fail_pipe[1], F_SETFD, FD_CLOEXEC)) < 0)
            goto fail;

        execvp(argv[0], const_cast<char* const*>(argv.data()));
fail:
        int fail_code = errno;
        CCSH_RETRY_HANDLER(write(fail_pipe[1], &fail_code, sizeof(int)));
        _exit(-1);
    }
    else
    {
        close_fd(fail_pipe[1]);
        open_wrapper temp(fail_pipe[0]);

        int fail_code;
        ssize_t result = CCSH_RETRY_HANDLER(read(fail_pipe[0], &fail_code, sizeof(int)));
        stdc_thrower(result);

        if (result > 0)
            throw stdc_error(fail_code, p.string());

        auto f = [pid]
        {
            int status;
            while (CCSH_RETRY_HANDLER(waitpid(pid, &status, 0)) >= 0)
            {
                if (WIFEXITED(status))
                    return WEXITSTATUS(status);
                if (WIFSIGNALED(status))
                    return -WTERMSIG(status);
            }
            return 0;
        };

        this->result = std::async(std::launch::async, f);
    }
}

int command_native::finish_run() const
{
    return result.get();
}

void command_pipe::start_run(int in, int out, int err, std::vector<int> left_unused_fds) const
{
    std::vector<int> right_unused_fds = left_unused_fds;
    int pipefd[2];
    stdc_thrower(pipe(pipefd));
    open_wrapper temp0{pipefd[0]};
    open_wrapper temp1{pipefd[1]};
    left_unused_fds.push_back(pipefd[0]);
    right_unused_fds.push_back(pipefd[1]);
    left.start_run(in, pipefd[1], err, std::move(left_unused_fds));
    right.start_run(pipefd[0], out, err, std::move(right_unused_fds));
}

int command_pipe::finish_run() const
{
    int result1 = left.finish_run();
    int result2 = right.finish_run();
    return shell_logic_or(result1, result2);
}

void command_in_mapping::start_run(int, int out, int err, std::vector<int> unused_fds) const
{
    if (init_func) init_func();

    int pipefd[2];
    stdc_thrower(pipe(pipefd));
    open_wrapper temp0{pipefd[0]};
    open_wrapper temp1{pipefd[1]};

    unused_fds.push_back(pipefd[1]);
    auto f2 = [this, pipefd](open_wrapper fd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while ((count = func(buf, BUFSIZ)) > 0)
            stdc_thrower(CCSH_RETRY_HANDLER(write(fd.get(), buf, std::size_t(count))));

        return 0;
    };

    c.start_run(pipefd[0], out, err, std::move(unused_fds));
    result = std::async(std::launch::async, f2, std::move(temp1));
}

void command_out_mapping::start_run(int in, int, int err, std::vector<int> unused_fds) const
{
    if (init_func) init_func();

    int pipefd[2];
    stdc_thrower(pipe(pipefd));
    open_wrapper temp0{pipefd[0]};
    open_wrapper temp1{pipefd[1]};

    unused_fds.push_back(pipefd[0]);
    auto f2 = [this, pipefd](open_wrapper fd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while ((count = read(fd.get(), buf, BUFSIZ)) > 0)
            func(buf, std::size_t(count));

        stdc_thrower(count);

        return 0;
    };

    c.start_run(in, pipefd[1], err, std::move(unused_fds));
    close_fd(pipefd[1]);
    result = std::async(std::launch::async, f2, std::move(temp0));
}

void command_err_mapping::start_run(int in, int out, int, std::vector<int> unused_fds) const
{
    if (init_func) init_func();

    int pipefd[2];
    stdc_thrower(pipe(pipefd));
    unused_fds.push_back(pipefd[0]);
    open_wrapper temp0{pipefd[0]};
    open_wrapper temp1{pipefd[1]};

    auto f2 = [this, pipefd](open_wrapper fd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while ((count = read(fd.get(), buf, BUFSIZ)) > 0)
            func(buf, std::size_t(count));

        stdc_thrower(count);

        return 0;
    };

    c.start_run(in, out, pipefd[1], std::move(unused_fds));
    close_fd(pipefd[1]);
    result = std::async(std::launch::async, f2, std::move(temp0));
}

template<stdfd DESC>
command_redirect<DESC>::command_redirect(command c, fs::path p, bool append)
    : c(std::move(c))
    , p(std::move(p))
    , flags(fopen_flags(DESC, append))
{ }

template<stdfd DESC>
void command_redirect<DESC>::start_run(int in, int out, int err, std::vector<int> unused_fds) const
{
    fd = open_wrapper{open(p.c_str(), flags, fopen_w_mode)};
    int fds[int(stdfd::count)] = {in, out, err};
    fds[int(DESC)] = fd.get();
    c.start_run(fds[int(stdfd::in)], fds[int(stdfd::out)], fds[int(stdfd::err)], std::move(unused_fds));
}

template
class command_redirect<stdfd::in>;

template
class command_redirect<stdfd::out>;

template
class command_redirect<stdfd::err>;


template<stdfd DESC>
command_fd<DESC>::command_fd(command c, int fd)
    : c(std::move(c))
    , ow(fd)
{ }

template<stdfd DESC>
void command_fd<DESC>::start_run(int in, int out, int err, std::vector<int> unused_fds) const
{
    int fds[int(stdfd::count)] = {in, out, err};
    fds[int(DESC)] = ow.get();
    c.start_run(fds[int(stdfd::in)], fds[int(stdfd::out)], fds[int(stdfd::err)], std::move(unused_fds));
}

template
class command_fd<stdfd::in>;

template
class command_fd<stdfd::out>;

template
class command_fd<stdfd::err>;


namespace {
void replace(std::string& str, std::string const& from, std::string const& to)
{
    std::size_t start_pos;
    while ((start_pos = str.find(from)) != std::string::npos)
        str.replace(start_pos, from.length(), to);
}

std::string sh_escape(std::string const& str)
{
    std::string temp = str;
    replace(temp, "'", R"('\'')");
    return " '" + temp + "' ";
}

std::string make_source_command(fs::path const& p, std::vector<std::string> const& args)
{
    std::string cmdstr = "source " + sh_escape(p.string());
    for (std::string const& str : args)
        cmdstr += sh_escape(str);

    cmdstr += " && (printenv -0) >&";
    return cmdstr;
}

void env_putter(std::string const& str)
{
    auto eq_sign = str.find('=');
    if (eq_sign == std::string::npos) // should never happen
        throw stdc_error(errno, "Bad format from printenv");

    std::string env_name = str.substr(0, eq_sign);
    std::string env_value = str.substr(++eq_sign);

    stdc_thrower(setenv(env_name.c_str(), env_value.c_str(), int(true)));
}

auto env_applier = [](open_wrapper fd) -> int
{
    auto env_splitter = line_splitter_make(env_putter, '\0');

    char buf[BUFSIZ];
    ssize_t count;
    while ((count = read(fd.get(), buf, BUFSIZ)) > 0)
        env_splitter(buf, std::size_t(count));

    return 0;
};

}  // namespace

command_source::command_source(fs::path const& p, std::vector<std::string> const& args)
    : cmd("/bin/sh", {"-c", ""})
    , cmdstr(make_source_command(p, args))
{ }


void command_source::start_run(int in, int out, int err, std::vector<int> unused_fds) const
{
    int pipefd[2];
    stdc_thrower(pipe(pipefd));
    open_wrapper temp0{pipefd[0]};
    open_wrapper temp1{pipefd[1]};

    unused_fds.push_back(pipefd[0]);
    cmd.args[1] = cmdstr + std::to_string(pipefd[1]);
    cmd.start_run(in, out, err, std::move(unused_fds));

    result = std::async(std::launch::async, env_applier, std::move(temp0));
}

void command_function::start_run(int in, int out, int err, std::vector<int>) const
{
    open_wrapper in2{CCSH_RETRY_HANDLER(fcntl(in, F_DUPFD_CLOEXEC, 0))};
    open_wrapper out2{CCSH_RETRY_HANDLER(fcntl(out, F_DUPFD_CLOEXEC, 0))};
    open_wrapper err2{CCSH_RETRY_HANDLER(fcntl(err, F_DUPFD_CLOEXEC, 0))};

    struct dup_helper // working around missing [=, temp0 = std::move(in2)] feature in C++11
    {
        open_wrapper in2;
        open_wrapper out2;
        open_wrapper err2;
        command_function const* self;

        int operator()() const
        {
            return self->func(in2.get(), out2.get(), err2.get());
        }
    };

    dup_helper helper{std::move(in2), std::move(out2), std::move(err2), this};
    result = std::async(std::launch::async, std::move(helper));
}

} // namespace internal
} // namespace ccsh

