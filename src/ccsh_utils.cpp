#include "ccsh_utils.hpp"

#include <sys/types.h>
#include <pwd.h>

#include <memory>

namespace ccsh
{

fs::path get_home()
{
    struct passwd pwd;
    struct passwd *result;

    long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)
        bufsize = 16384;

    std::unique_ptr<char[]> buf{new char[bufsize]};

    if(getpwuid_r(getuid(), &pwd, buf.get(), bufsize, &result) != 0 ||
        result == nullptr ||
        result->pw_dir == nullptr)
    {
        throw stdc_error();
    }

    return fs::path{result->pw_dir};
}

const char * stdc_error::what() const noexcept
{
    return strerror(error_number);
}

void open_traits::dtor_func(int fd) noexcept
{
    if(fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
        close_fd(fd);
}

void close_fd(int fd) noexcept
{
    while(close(fd) == -1 && errno == EINTR)
        ;
}

env_var::operator std::string() const
{
    const char* result = getenv(name.c_str());
    return result == nullptr ? "" : result;
}

env_var& env_var::operator=(std::string const& str)
{
    stdc_thrower(setenv(name.c_str(), str.c_str(), true));
    return *this;
}

} // namespace ccsh
