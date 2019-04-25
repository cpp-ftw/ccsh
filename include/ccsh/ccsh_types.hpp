#ifndef CCSH_CCSH_TYPES_HPP
#define CCSH_CCSH_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <string>

namespace ccsh {

class winapi_error;
class stdc_error;

#ifdef _WIN32

using error_t = uint32_t;   // DWORD
using fd_t = void*;         // HANDLE

#ifndef _UNICODE
#error Please compile Windows sources with _UNICODE!
#endif

using tchar_t = wchar_t;    // TCHAR

#define CCSH_TEXT(x) L##x

using fd_exception_t = winapi_error;

#else

using error_t = int;
using fd_t = int;
using tchar_t = char;

using fd_exception_t = stdc_error;

#define CCSH_TEXT(x) x

#endif // _WIN32

using tstring_t = std::basic_string<tchar_t>;

}

#endif
