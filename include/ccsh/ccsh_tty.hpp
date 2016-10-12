#ifndef CCSH_TTY_HPP_INCLUDED
#define CCSH_TTY_HPP_INCLUDED

#include <iostream>
#include <cstdint>

namespace ccsh { namespace tty {

enum class mode : uint8_t
{
    normal     = 0,
    bold       = 1,
    dim        = 2,
    underline  = 3,
    underscore = 4,
    blink      = 5,
    reverse    = 7,
    concealed  = 8,
};

enum class fg_color : uint8_t
{
    black   =  30,
    red     =  31,
    green   =  32,
    yellow  =  33,
    blue    =  34,
    magenta =  35,
    cyan    =  36,
    white   =  37,
};

enum class bg_color : uint8_t
{
    black   =  40,
    red     =  41,
    green   =  42,
    yellow  =  43,
    blue    =  44,
    magenta =  45,
    cyan    =  46,
    white   =  47,
};

template<typename ENUM>
std::ostream& printty(std::ostream& os, ENUM e)
{
	os << "\x1B[" << int(e) << 'm';
	return os;
}

inline std::ostream& operator<<(std::ostream& os, tty::mode m)
{
	return printty(os, m);
}

inline std::ostream& operator<<(std::ostream& os, tty::fg_color m)
{
	return printty(os, m);
}
inline std::ostream& operator<<(std::ostream& os, tty::bg_color m)
{
	return printty(os, m);
}


} }



#endif // CCSH_TTY_HPP_INCLUDED