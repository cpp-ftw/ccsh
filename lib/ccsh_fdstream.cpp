#include <ccsh/ccsh_fdstream.hpp>
#include <cstdio>
#include <unistd.h>

namespace ccsh { namespace internal {

int ofdstreambuf::overflow(int_type c)
{
    if (c == traits_type::eof())
        return traits_type::eof();

    char_type ch = traits_type::to_char_type(c);
    xsputn(&ch, 1);
    return 1;
}

std::streamsize ofdstreambuf::xsputn(const char_type* s, std::streamsize n)
{
    return ::write(fd, s, size_t(n));
}

}}

