#include <ccsh/ccsh_utils.hpp>
#include "ccsh_internals.hpp"

#ifdef _WIN32

#ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#endif
#include <codecvt>

#include <mutex>

namespace ccsh {

fs::path get_home()
{
    tchar_t result[MAX_PATH];
    winapi_hresult_thrower(SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, 0, result));
    return {result};
}

std::string get_hostname()
{
    tchar_t result[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD sz = sizeof(result);
    winapi_thrower(GetComputerNameW(result, &sz));
    return to_utf8(result);
}

std::string get_ttyname()
{
    tchar_t result[MAX_PATH];
    winapi_thrower(GetConsoleTitleW(result, MAX_PATH));
    return to_utf8(result);
}

bool is_user_possibly_elevated()
{
    fd_t temp = nullptr;
    winapi_thrower(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &temp));
    internal::open_wrapper hToken{temp};
    TOKEN_ELEVATION Elevation;
    DWORD cbSize = sizeof(Elevation);
    winapi_thrower(GetTokenInformation(hToken.get(), TokenElevation, &Elevation, cbSize, &cbSize));
    return Elevation.TokenIsElevated;
}

std::string get_error_as_string(error_t errorMessageID)
{
    if (errorMessageID == 0)
        return "Success";

    tchar_t* messageBuffer = nullptr;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<tchar_t*>(&messageBuffer), 0, nullptr);

    tstring_t message(messageBuffer, size);
    LocalFree(messageBuffer);
    while (!message.empty() && message.back() == L'\n')
        message.pop_back();

    return to_utf8(message);
}

winapi_error::winapi_error()
    : winapi_error(GetLastError())
{}

winapi_error::winapi_error(error_t no)
    : shell_error(get_error_as_string(no))
    , error_number(no)
{}

winapi_error::winapi_error(std::string const & msg)
    : winapi_error(GetLastError(), msg)
{}

winapi_error::winapi_error(error_t no, std::string const & msg)
    : shell_error(msg.empty() ? get_error_as_string(no) : msg + ": " + get_error_as_string(no))
    , error_number(no)
{}

std::string const& stdc_error::strerror(int no)
{
    static thread_local std::string result(BUFSIZ, '\0');
    // Let's hope strerror_s can't fail unless buffer is too small.
    // Anyway, if buffer is small, it returns EINVAL (?)
    int err;
    while ((err = strerror_s(&result[0], result.size(), no)) == EINVAL)
        result.resize(result.size() * 2);

    if (err != EINVAL) // should not happen
        result = "An unknown error occured in strerror_s.";

    // Removing trailing '\n'. Assuming that if it contains a newline,
    // it won't be in the middle of the string. Looking at this wonderful API,
    // this assumption is risky.
    auto pos = result.find_last_of('\n');
    if (pos != std::string::npos)
        result[pos] = '\0';

    return result;
}


namespace {

static std::mutex env_var_mtx;

bool get_env_var(std::string const& name, std::string& result)
{
    std::lock_guard<std::mutex> guard{env_var_mtx};
    std::size_t retsize;
    getenv_s(&retsize, &result[0], result.size(), name.c_str());
    if (retsize == 0)
        return false;

    if (retsize > result.size())
    {
        result = std::string(retsize, '\0');
        getenv_s(&retsize, &result[0], result.size(), name.c_str());
    }
    return true;
}

}

std::string env_var::get(std::string const& name)
{
    std::string result(BUFSIZ, '\0');
    if (!get_env_var(name, result))
        throw shell_error("Environment variable " + name + " does not exist.");
    return result;
}

const char* env_var::try_get(std::string const& name)
{
    static thread_local std::string result(BUFSIZ, '\0');
    if (!get_env_var(name, result))
        return nullptr;
    return result.c_str();
}

void env_var::set(std::string const& name, std::string const& value, bool override)
{
    internal::stdc_thrower(try_set(name, value, override));
}

int env_var::try_set(std::string const& name, std::string const& value, bool overwrite)
{
    std::lock_guard<std::mutex> guard{env_var_mtx};
    int errcode = 0;
    if (!overwrite)
    {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, nullptr, 0, name.c_str());
        if (errcode || envsize) return errcode;
    }
    errno_t err = _putenv_s(name.c_str(), value.c_str());
    if (err)
    {
        errno = err;
        return -1;
    }
    return 0;
}

namespace internal {

const fd_t open_traits::invalid_value = INVALID_HANDLE_VALUE;

void open_traits::dtor_func(fd_t fd) noexcept
{
    // let's assume these won't be changed
    static fd_t handle1 = GetStdHandle(STD_INPUT_HANDLE);
    static fd_t handle2 = GetStdHandle(STD_OUTPUT_HANDLE);
    static fd_t handle3 = GetStdHandle(STD_ERROR_HANDLE);

    if (fd != handle1 && fd != handle2 && fd != handle3)
        CloseHandle(fd);
}

pipe_t pipe_compat()
{
    SECURITY_ATTRIBUTES secattr = {0};
    secattr.nLength = sizeof(secattr);
    secattr.lpSecurityDescriptor = nullptr;
    secattr.bInheritHandle = inherit_by_default();

    HANDLE read_end;
    HANDLE write_end;
    winapi_thrower(CreatePipe(&read_end, &write_end, &secattr, 0));
    return {open_wrapper{read_end}, open_wrapper{write_end}};
}

std::size_t read_compat(fd_t file, void* data, std::size_t size)
{
    DWORD result;
    winapi_thrower(ReadFile(file, data, unsigned(size), &result, nullptr));
    return std::size_t(result);
}

std::size_t write_compat(fd_t file, void* data, std::size_t size)
{
    DWORD result;
    winapi_thrower(WriteFile(file, data, unsigned(size), &result, nullptr));
    return std::size_t(result);
}

fd_t duplicate_compat(fd_t file)
{
    fd_t result;
    winapi_thrower(DuplicateHandle(GetCurrentProcess(), file, GetCurrentProcess(), &result, 0, false, DUPLICATE_SAME_ACCESS));
    return result;
}

void close_compat(fd_t fd)
{
    CloseHandle(fd);
}


} // namespace internal

std::string to_utf8(tstring_t const& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(str);
}

tstring_t from_utf8(std::string const& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}

}

#endif // _WIN32
