#ifndef CCSH_INTERNALS_POSIX_HPP_INCLUDED
#define CCSH_INTERNALS_POSIX_HPP_INCLUDED

#ifndef _WIN32

#include <ccsh/ccsh_utils.hpp>

#include <string>
#include <type_traits>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <fcntl.h>

namespace ccsh {
namespace internal {

template<typename FUNC>
auto retry_handler(FUNC&& func) -> decltype(func())
{
    int result;
    int err;
    while ((result = func()) == -1 && ((err = errno), err == EINTR || err == EAGAIN || err == EWOULDBLOCK));
    return result;
}

#define CCSH_RETRY_HANDLER(operation) retry_handler([&]() { return (operation); })

inline void close_fd(int fd) noexcept
{
    CCSH_RETRY_HANDLER(close(fd));
}

constexpr mode_t fopen_w_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

constexpr int fopen_flags(stdfd fd, bool append = false)
{
    return fd == stdfd::in ? O_RDONLY :
           (append ?
            (O_WRONLY | O_CREAT | O_APPEND) :
            (O_WRONLY | O_CREAT | O_TRUNC));
}

inline std::string&& to_utf8(std::string&& str)
{
    return std::move(str);
}

inline std::string const& to_utf8(std::string const& str)
{
    return str;
}

inline std::string& to_utf8(std::string& str)
{
    return str;
}

} // namespace internal
} // namespace ccsh

#endif // _WIN32

#endif // CCSH_INTERNALS_POSIX_HPP_INCLUDED
