#ifndef CCSH_INTERNALS_WINDOWS_HPP_INCLUDED
#define CCSH_INTERNALS_WINDOWS_HPP_INCLUDED

#ifdef _WIN32

#include <ccsh/ccsh_utils.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h>
#include <VersionHelpers.h>

#ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#endif
#include <codecvt>

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
    if ((hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0))
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

template<std::size_t N>
std::string to_utf8(const wchar_t(&str)[N])
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(std::begin(str), std::end(str));
}

inline std::string to_utf8(std::wstring const& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(str);
}

template<std::size_t N>
std::wstring from_utf8(const char (&str)[N])
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(std::begin(str), std::end(str));
}

inline std::wstring from_utf8(std::string const& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
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