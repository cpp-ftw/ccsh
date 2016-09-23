#include "ccsh_command.hpp"

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
    int pipefd[2];

    stdc_thrower(pipe(pipefd));

    pid_t pid = fork();
    stdc_thrower(pid);

    if (pid == 0) /* Child process */
    {
        close(pipefd[!child_action]);
        int result0 = func_child(pipefd[child_action]);
        close(pipefd[child_action]);
        _exit(result0);
    }
    else /* Parent process */
    {
        close(pipefd[child_action]);
        int result1 = func_parent(pipefd[!child_action]);
        close(pipefd[!child_action]);

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
        std::cerr << "An unhandled type of exceptin was thrown in command::autorun" << std::endl;
    }
}

command_native::command_native(std::string const& str, std::vector<std::string> const& args)
    : str(str)
    , args(args)
{
    argv.reserve(args.size() + 2);

    argv.push_back(this->str.c_str());
    for(const auto& s : this->args)
        argv.push_back(s.c_str());

    argv.push_back(nullptr);
}


int command_native::runx(int in, int out, int err) const
{
    pid_t pid = vfork();
    stdc_thrower(pid);

    if(pid == 0)
    {
        if(in != STDIN_FILENO)
            dup2(in, STDIN_FILENO);

        if(out != STDOUT_FILENO)
            dup2(out, STDOUT_FILENO);

        if(err != STDERR_FILENO)
            dup2(err, STDERR_FILENO);

        execvp(argv[0], (char*const*)argv.data());
        _exit(-1);
    }
    else
    {
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
    auto f1 = std::bind(&command_runnable::runx, std::ref(right), _1, out, err);
    auto f2 = std::bind(&command_runnable::runx, std::ref(left),  in,  _1, err);

    auto p = fork_functor_helper(FORK_CHILD_READ, f1, f2);
    return shell_logic_or(p.first, p.second);
}

int command_in_mapping::runx(int, int out, int err) const
{
    auto f1 = std::bind(&command_runnable::runx, std::ref(c), _1, out, err);

    auto f2 = [this](int pipefd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while((count = func(buf, BUFSIZ)) > 0)
            write(pipefd, buf, count); // error handling?!

        return 0;
    };

    return fork_functor_helper(FORK_CHILD_READ, f1, f2).first;
}

int command_out_mapping::runx(int in, int, int err) const
{
    auto f1 = std::bind(&command_runnable::runx, std::ref(c), in, _1, err);

    auto f2 = [this](int pipefd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while((count = read(pipefd, buf, BUFSIZ)) > 0)
            func(buf, count);

        return 0;
    };

    return fork_functor_helper(FORK_CHILD_WRITE, f1, f2).first;
}

int command_err_mapping::runx(int in, int out, int) const
{
    auto f1 = std::bind(&command_runnable::runx, std::ref(c), in, out, _1);

    auto f2 = [this](int pipefd)
    {
        char buf[BUFSIZ];
        ssize_t count;
        while((count = read(pipefd, buf, BUFSIZ)) > 0)
            func(buf, count);

        return 0;
    };

    return fork_functor_helper(FORK_CHILD_WRITE, f1, f2).first;
}

command_redirect::command_redirect(command_runnable const& c, fs::path const& p, int flags)
    : c(c)
    , p(p)
    , flags(flags)
{ }

open_wrapper command_redirect::get_fd() const
{
    return open_wrapper{open(p.c_str(), flags, fopen_w_mode)};
}

command_in_redirect::command_in_redirect(command_runnable const& c, fs::path const& p)
    : command_redirect(c, p, O_RDONLY)
{ }

command_out_redirect::command_out_redirect(command_runnable const& c, fs::path const& p, bool append)
    : command_redirect(c, p, fopen_w_flags(append))
{ }

command_err_redirect::command_err_redirect(command_runnable const& c, fs::path const& p, bool append)
    : command_redirect(c, p, fopen_w_flags(append))
{ }


} // namespace ccsh

