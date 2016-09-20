#include "ccsh.hpp"

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

namespace ccsh
{

std::string pwd()
{
    char * wd = get_current_dir_name();
    std::string result = wd;
    free(wd);
    return result;
}

const char * stdc_error::what() const noexcept
{
    return strerror(error_number);
}

void open_traits::dtor_func(int fd) noexcept
{
    if(fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
        close(fd);
}

int command_base::run() const
{
    return runx(STDIN_FILENO, STDOUT_FILENO);
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


int command_native::runx(int in, int out) const
{
    pid_t pid = fork();
    stdc_thrower(pid);

    if(pid == 0)
    {
        if(out != STDOUT_FILENO)
            dup2(out, STDOUT_FILENO);

        if(in != STDIN_FILENO)
            dup2(in, STDIN_FILENO);

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


int command_pipe::runx(int in, int out) const
{
    int pipefd[2];

    stdc_thrower(pipe(pipefd));

    pid_t pid = fork();
    stdc_thrower(pid);

    if (pid == 0)
    {    /* Child reads from pipe */
        close(pipefd[1]);          /* Close unused write end */

        // read(pipefd[0], &buf, 1)
        right->runx(pipefd[0], out);

        close(pipefd[0]);
    }
    else
    {            /* Parent writes argv[1] to pipe */
        close(pipefd[0]);          /* Close unused read end */

        int result = left->runx(in, pipefd[1]);

        close(pipefd[1]);          /* Reader will see EOF */

        return result;
    }
    return 0;
}

command_out_redirect::command_out_redirect(command c, std::string const& path)
    : c(c)
    , fd(open(path.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR))
{ }

} // namespace ccsh
