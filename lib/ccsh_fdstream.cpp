#include "ccsh_internals.hpp"
#include <ccsh/ccsh_fdstream.hpp>

#include <cstdio>
#include <cstring>
#include <unistd.h>

namespace ccsh {
namespace internal {

// Code in this file is mostly taken from this article:
// http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
// which is licensed under Boost Software License 1.0.

bool ofdstreambuf::do_flush()
{
    std::ptrdiff_t n = pptr() - pbase();
    pbump(int(-n));

    return CCSH_RETRY_HANDLER(::write(fd, pbase(), size_t(n))) != -1;
}

int ofdstreambuf::overflow(int_type c)
{
    if (c != traits_type::eof())
    {
        char ch = traits_type::to_char_type(c);
        *pptr() = ch;
        pbump(1);
        if (do_flush())
            return ch;
    }

    return traits_type::eof();
}

int ifdstreambuf::underflow()
{
    if (gptr() < egptr()) // buffer not exhausted
        return traits_type::to_int_type(*gptr());

    char* base = &buffer_.front();
    char* start = base;

    if (eback() == base) // true when this isn't the first fill
    {
        // Make arrangements for putback characters
        std::memmove(base, egptr() - put_back_size, put_back_size);
        start += put_back_size;
    }

    // start is now the start of the buffer, proper.
    // Read from fptr_ in to the provided buffer
    ssize_t n = read(fd, start, buffer_.size() - (start - base));

    if (n == 0)
        return traits_type::eof();

    // Set buffer pointers
    setg(base, start, start + n);

    return traits_type::to_int_type(*gptr());
}

} // namespace internal
} // namespace ccsh

