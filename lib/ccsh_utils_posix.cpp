#include <ccsh/ccsh_utils.hpp>
#include "ccsh_internals.hpp"

#ifndef _WIN32

#include <glob.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>

namespace ccsh {

fs::path get_home()
{
    struct passwd pwd;
    struct passwd* result;

    auto bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)
        bufsize = 16384;

    std::unique_ptr<char[]> buf{ new char[bufsize] };

    if (getpwuid_r(getuid(), &pwd, buf.get(), std::size_t(bufsize), &result) != 0 ||
        result == nullptr ||
        result->pw_dir == nullptr)
    {
        throw stdc_error();
    }

    return fs::path{ result->pw_dir };
}

std::string get_hostname()
{
    char buf[HOST_NAME_MAX + 1];
    internal::stdc_thrower(gethostname(buf, sizeof(buf)));
    return buf;
}

std::string get_ttyname()
{
    fs::path result = ttyname(fileno(stdin));
    return result.string();
}


bool is_user_possibly_elevated()
{
    uid_t uid = getuid();
    uid_t euid = geteuid();

    return uid <= 0 || uid != euid;
}

std::string const& stdc_error::strerror(int no)
{
    static thread_local std::string result(BUFSIZ, '\0');
#if (_POSIX_C_SOURCE >= 200112L) && ! _GNU_SOURCE
    int err;
    while ((err = strerror_r(no, &result[0], result.size())) == ERANGE)
        result.resize(result.size() * 2);
#else
    while (strerror_r(no, &result[0], result.size()) != &result[0] && errno == ERANGE)
        result.resize(result.size() * 2);
    int err = errno;
#endif
    if (err != ERANGE) // should not happen
        result = "An unknown error occured in strerror_r.";

    return result;
}

std::string env_var::get(std::string const& name)
{
    const char * result = std::getenv(name.c_str());
    if (result == nullptr)
        throw shell_error("Environment variable " + name + " does not exist.");
    return result;
}

const char* env_var::try_get(std::string const& name)
{
    return std::getenv(name.c_str());
}

void env_var::set(std::string const& name, std::string const& value, bool override)
{
    internal::stdc_thrower(try_set(name, value, override));
}

int env_var::try_set(std::string const& name, std::string const& value, bool override)
{
    return setenv(name.c_str(), value.c_str(), int(override));
}

namespace internal {

void open_traits::dtor_func(fd_t fd) noexcept
{
    if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
        close_fd(fd);
}

pipe_t pipe_compat()
{
    int pipefd[2];
    stdc_thrower(pipe(pipefd));
    return {open_wrapper{pipefd[0]}, open_wrapper{pipefd[1]}};
}

std::size_t read_compat(fd_t file, void* data, std::size_t size)
{
    ssize_t result = CCSH_RETRY_HANDLER(read(file, data, size));
    stdc_thrower(result);
    return std::size_t(result);
}

std::size_t write_compat(fd_t file, void* data, std::size_t size)
{
    ssize_t result = CCSH_RETRY_HANDLER(write(file, data, size));
    stdc_thrower(result);
    return std::size_t(result);
}

void close_compat(fd_t fd)
{
    close_fd(fd);
}

fd_t duplicate_compat(fd_t file)
{
    int result = CCSH_RETRY_HANDLER(fcntl(file, F_DUPFD_CLOEXEC, 0));
    stdc_thrower(result);
    return result;
}

static_assert(int(stdfd::in) == STDIN_FILENO, "Error in stdfd enum.");
static_assert(int(stdfd::out) == STDOUT_FILENO, "Error in stdfd enum.");
static_assert(int(stdfd::err) == STDERR_FILENO, "Error in stdfd enum.");

} // namespace internal


} // namespace ccsh

#endif // _WIN32