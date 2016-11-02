#include <ccsh/ccsh_command.hpp>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <utility>

namespace ccsh
{

namespace
{

constexpr mode_t fopen_w_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

constexpr int fopen_w_flags(bool append)
{
    return append ?
        (O_WRONLY | O_CREAT | O_APPEND) :
        (O_WRONLY | O_CREAT | O_TRUNC);
}

enum fork_action
{
    FORK_CHILD_READ = 0,
    FORK_CHILD_WRITE = 1
};

template<typename FUNC_CHILD, typename FUNC_PARENT>
std::pair<int, int> fork_functor_helper(fork_action child_action, FUNC_CHILD const& func_child, FUNC_PARENT const& func_parent)
{
    int fail_pipe[2];
    stdc_thrower(pipe(fail_pipe));

    int pipefd[2];
    stdc_thrower(pipe(pipefd));

    pid_t pid = fork();
    stdc_thrower(pid);

    if (pid == 0) /* Child process */
    {
        int fail_code;
        close_fd(pipefd[!child_action]);
        close_fd(fail_pipe[0]);

        if(fcntl(fail_pipe[1], F_SETFD, FD_CLOEXEC) < 0)
        {
            fail_code = errno;
            goto fail;
        }

        try
        {
            int result0 = func_child(pipefd[child_action]);
            close_fd(fail_pipe[1]);
            _exit(result0);
        }
        catch(stdc_error const& x)
        {
            fail_code = x.no();
        }

fail:
        write(fail_pipe[1], &fail_code, sizeof(int));
        close_fd(fail_pipe[1]);
        _exit(-1);
    }
    else /* Parent process */
    {
        close_fd(fail_pipe[1]);
        open_wrapper temp1(fail_pipe[0]);

        close_fd(pipefd[child_action]);
        open_wrapper temp2(pipefd[!child_action]);

        int fail_code;
        ssize_t result = read(fail_pipe[0], &fail_code, sizeof(int));
        stdc_thrower(result);

        if(result > 0)
            throw stdc_error(fail_code);

        int result1 = func_parent(pipefd[!child_action]);
        close_fd(pipefd[!child_action]);

        int status;
        if(waitpid(pid, &status, 0) < 0 || WIFEXITED(status) || WIFSIGNALED(status))
        {
            return {WEXITSTATUS(status), result1};
        }

        return {0, result1};
    }
}

} // namespace

using namespace std::placeholders;

int command_base::run() const
{
    no_autorun();
    return runx(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
}

void command_base::run_autorun() noexcept
{
    if(!autorun_flag)
        return;

    try
    {
        run();
    }
    catch(std::exception const& x)
    {
        std::cerr << x.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "An unhandled type of exception was thrown in command::autorun" << std::endl;
    }
}


int command_native::runx(int in, int out, int err) const
{
    int fail_pipe[2];
    stdc_thrower(pipe(fail_pipe));

    auto argv = get_argv();

    pid_t pid = vfork();
    stdc_thrower(pid);

    if(pid == 0)
    {
        close_fd(fail_pipe[0]);

        if(in != STDIN_FILENO)
            if(dup2(in, STDIN_FILENO) < 0)
                goto fail;

        if(out != STDOUT_FILENO)
            if(dup2(out, STDOUT_FILENO) < 0)
                goto fail;

        if(err != STDERR_FILENO)
            if(dup2(err, STDERR_FILENO) < 0)
                goto fail;

        if(fcntl(fail_pipe[1], F_SETFD, FD_CLOEXEC) < 0)
            goto fail;

        execvp(argv[0], (char*const*)argv.data());
fail:
        int fail_code = errno;
        write(fail_pipe[1], &fail_code, sizeof(int));
        _exit(-1);
    }
    else
    {
        close_fd(fail_pipe[1]);
        open_wrapper temp(fail_pipe[0]);

        int fail_code;
        ssize_t result = read(fail_pipe[0], &fail_code, sizeof(int));
        stdc_thrower(result);

        if(result > 0)
            throw stdc_error(fail_code, str);

        int status;
        if(waitpid(pid, &status, 0) < 0 || WIFEXITED(status) || WIFSIGNALED(status))
        {
            return WEXITSTATUS(status);
        }
    }
    return 0;
}

int command_pipe::runx(int in, int out, int err) const
{
    auto f1 = std::bind(&command::runx, std::ref(left),  in,  _1, err);
    auto f2 = std::bind(&command::runx, std::ref(right), _1, out, err);

    auto p = fork_functor_helper(FORK_CHILD_WRITE, f1, f2);
    return shell_logic_or(p.first, p.second);
}

int command_in_mapping::runx(int, int out, int err) const
{
    if(init_func) init_func();
    auto f1 = std::bind(&command::runx, std::ref(c), _1, out, err);

    auto f2 = [this](int pipefd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while((count = func(buf, BUFSIZ)) > 0)
            stdc_thrower(write(pipefd, buf, count));

        return 0;
    };

    return fork_functor_helper(FORK_CHILD_WRITE, f2, f1).first;
}

int command_out_mapping::runx(int in, int, int err) const
{
    if(init_func) init_func();
    auto f1 = std::bind(&command::runx, std::ref(c), in, _1, err);

    auto f2 = [this](int pipefd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while((count = read(pipefd, buf, BUFSIZ)) > 0)
            func(buf, count);

        stdc_thrower(count);

        return 0;
    };

    return fork_functor_helper(FORK_CHILD_WRITE, f1, f2).first;
}

int command_err_mapping::runx(int in, int out, int) const
{
    if(init_func) init_func();
    auto f1 = std::bind(&command::runx, std::ref(c), in, out, _1);

    auto f2 = [this](int pipefd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while((count = read(pipefd, buf, BUFSIZ)) > 0)
            func(buf, count);

        stdc_thrower(count);

        return 0;
    };

    return fork_functor_helper(FORK_CHILD_WRITE, f1, f2).first;
}

command_redirect::command_redirect(command const& c, fs::path const& p, int flags)
    : c(c)
    , p(p)
    , flags(flags)
{ }

open_wrapper command_redirect::get_fd() const
{
    return open_wrapper{open(p.c_str(), flags, fopen_w_mode)};
}

command_in_redirect::command_in_redirect(command const& c, fs::path const& p)
    : command_redirect(c, p, O_RDONLY)
{ }

command_out_redirect::command_out_redirect(command const& c, fs::path const& p, bool append)
    : command_redirect(c, p, fopen_w_flags(append))
{ }

command_err_redirect::command_err_redirect(command const& c, fs::path const& p, bool append)
    : command_redirect(c, p, fopen_w_flags(append))
{ }


} // namespace ccsh

