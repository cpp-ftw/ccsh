#include <ccsh/ccsh_command.hpp>

#ifndef _WIN32

#include "ccsh_internals.hpp"

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

int command_base::run() const
{
    no_autorun();
    start_run(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, std::vector<fd_t>());
    int result = finish_run();
    last_exit_code = result;
    return result;
}

command_native::command_native(fs::path p, std::vector<std::string> args)
    : p(std::move(p))
    , args_(std::move(args))
{ }

void command_native::add_arg(const char* str)
{
    args_.emplace_back(str);
}
void command_native::add_arg(std::string const& str)
{
    args_.push_back(str);
}
void command_native::add_arg(std::string&& str)
{
    args_.push_back(std::move(str));
}

void command_native::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const
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

template<stdfd DESC>
command_redirect<DESC>::command_redirect(command c, fs::path p, bool append)
    : c(std::move(c))
    , p(std::move(p))
    , flags(fopen_flags(DESC, append))
{ }

template<stdfd DESC>
void command_redirect<DESC>::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const
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



namespace {
void replace(std::string& str, std::string const& from, std::string const& to)
{
    std::size_t start_pos;
    while ((start_pos = str.find(from)) != std::string::npos)
        str.replace(start_pos, from.length(), to);
}

std::string sh_escape(std::string str)
{
    replace(str, "'", R"('\'')");
    return " '" + str + "' ";
}

std::string make_source_command(fs::path const& p, std::vector<std::string> const& args)
{
    std::string cmdstr = ". " + sh_escape(p.string());
    for (std::string const& str : args)
        cmdstr += sh_escape(str);

    cmdstr += " && (printenv -0) >&";
    return cmdstr;
}

void env_putter(std::string str)
{
    auto eq_sign = str.find('=');
    if (eq_sign == std::string::npos) // should never happen
        throw stdc_error(errno, "Bad format from printenv");

    str[eq_sign] = '\0';
    stdc_thrower(setenv(&str[0], &str[eq_sign + 1], int(true)));
}

auto env_applier = [](open_wrapper fd) -> int
{
    auto env_splitter = line_splitter_make(&env_putter, '\0');

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

void command_source::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const
{
    int pipefd[2];
    stdc_thrower(pipe(pipefd));
    open_wrapper temp0{pipefd[0]};
    open_wrapper temp1{pipefd[1]};

    unused_fds.push_back(pipefd[0]);
    cmd.args()[1] = cmdstr + std::to_string(pipefd[1]);
    cmd.start_run(in, out, err, std::move(unused_fds));

    result = std::async(std::launch::async, env_applier, std::move(temp0));
}

}}

#endif // _WIN32
