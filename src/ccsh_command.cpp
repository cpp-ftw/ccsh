#include "ccsh_command.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <iostream>

namespace ccsh
{

static constexpr mode_t fopen_w_mode_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;


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
    catch(std::exception& x)
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
    int pipefd[2];

    stdc_thrower(pipe(pipefd));

    pid_t pid = fork();
    stdc_thrower(pid);

    if (pid == 0)
    {    /* Child reads from pipe */
        close(pipefd[1]);          /* Close unused write end */

        // read(pipefd[0], &buf, 1)
        int result = right.runx(pipefd[0], out, err);

        close(pipefd[0]);
        _exit(result);
    }
    else
    {            /* Parent writes argv[1] to pipe */
        close(pipefd[0]);          /* Close unused read end */

        int result = left.runx(in, pipefd[1], err);

        close(pipefd[1]);          /* Reader will see EOF */

        int status;
        if(waitpid(pid, &status, 0) < 0 || WIFEXITED(status) || WIFSIGNALED(status))
        {
            return shell_logic_or(WEXITSTATUS(status), result);
        }

        return 0;
    }
    return 0;
}

command_redirect::command_redirect(command_runnable const& c, fs::path const& p, int flags)
    : c(c)
    , fd(open(p.c_str(),
              flags,
              fopen_w_mode_flags))
{ }

command_in_redirect::command_in_redirect(command_runnable const& c, fs::path const& p)
    : command_redirect(c, p, O_RDONLY)
{ }

command_out_redirect::command_out_redirect(command_runnable const& c, fs::path const& p, bool append)
    : command_redirect(c, p,
                       append ?
                        (O_WRONLY | O_CREAT | O_APPEND) :
                        (O_WRONLY | O_CREAT | O_TRUNC))
{ }

command_err_redirect::command_err_redirect(command_runnable const& c, fs::path const& p, bool append)
    : command_redirect(c, p,
                       append ?
                        (O_WRONLY | O_CREAT | O_APPEND) :
                        (O_WRONLY | O_CREAT | O_TRUNC))
{ }


} // namespace ccsh

