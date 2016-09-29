#include "ccsh_utils.hpp"

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
