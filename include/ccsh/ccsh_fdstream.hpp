#ifndef CCSH_CCSH_FDSTREAM_HPP_INCLUDED
#define CCSH_CCSH_FDSTREAM_HPP_INCLUDED

#include <iostream>

namespace ccsh {

namespace internal {

class ofdstreambuf : public std::streambuf
{
    int fd;
public:
    explicit ofdstreambuf(int fd)
        : fd(fd)
    { }

    std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    int_type overflow(int_type c) override;
};
}

class ofdstream : public std::ostream
{
public:
    explicit ofdstream(int fd)
        : std::ostream{new internal::ofdstreambuf{fd}}
    { }
};
}

#endif //CCSH_CCSH_FDSTREAM_HPP_INCLUDED
