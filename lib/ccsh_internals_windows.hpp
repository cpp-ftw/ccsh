#ifndef CCSH_INTERNALS_WINDOWS_HPP_INCLUDED
#define CCSH_INTERNALS_WINDOWS_HPP_INCLUDED

#ifdef _WIN32

#include <ccsh/ccsh_utils.hpp>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <Shlobj.h>
#include <VersionHelpers.h>

namespace ccsh {

template<typename ErrorType>
void winapi_thrower(ErrorType result)
{
    if (result == 0)
        throw winapi_error();
}

template<>
inline void winapi_thrower<fd_t>(fd_t result)
{
    if (result == INVALID_HANDLE_VALUE)
        throw winapi_error();
}

template<>
inline void winapi_thrower<DWORD>(DWORD result)
{
    if (result == 0xFFFFFFFF)
        throw winapi_error();
}

template<typename ErrorType>
void winapi_thrower(ErrorType result, std::string const& msg)
{
    if (result == 0)
        throw winapi_error(msg);
}

inline error_t from_hresult(HRESULT hr)
{
    if (HRESULT(hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0))
        return HRESULT_CODE(hr);

    if (hr == S_OK)
        return ERROR_SUCCESS;

    return ERROR_CAN_NOT_COMPLETE;
}

inline void winapi_hresult_thrower(HRESULT result) // DWORD
{
    if (FAILED(result))
        throw winapi_error(from_hresult(result));
}

inline bool inherit_by_default()
{
    // STARTUPINFOEX not supported below Vista, so handle inheritance had to be reworked.
    // XP workaround happens to be less exception-safe, so choose modern solution if possible.
    static bool result = IsWindowsVistaOrGreater();
    return result;
}

}

#endif // _WIN32

#endif // CCSH_INTERNALS_WINDOWS_HPP_INCLUDED
