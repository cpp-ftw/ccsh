#include "ccsh_internals.hpp"
#include <ccsh/ccsh_command.hpp>

#ifdef _WIN32

namespace ccsh {
namespace internal {

int command_base::run() const
{
    no_autorun();
    start_run(GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE), std::vector<fd_t>());
    int result = finish_run();
    last_exit_code = result;
    return result;
}

command_native::command_native(fs::path p, std::vector<std::string> args)
    : p(std::move(p))
{
    this->args_.reserve(args.size());
    for (const auto& str : args)
        this->args_.push_back(from_utf8(str));
}

void command_native::add_arg(const char* str)
{
    args_.push_back(from_utf8(str));
}
void command_native::add_arg(std::string const& str)
{
    args_.push_back(from_utf8(str));
}
void command_native::add_arg(std::string&& str)
{
    args_.push_back(from_utf8(str));
}

namespace {

tstring_t convert_argv(std::vector<const tchar_t*> const& argv)
{
    tstring_t result;

    for (auto it = argv.begin(); *it != nullptr; ++it)
    {
        auto arg = *it;

        result += L'"';
        int backslash_count = 0;
        for (int i = 0; arg[i] != '\0'; ++i)
        {
            switch (arg[i])
            {
            case L'\\':
                backslash_count++;
                break;
            case L'"':
                result.append(backslash_count * 2 + 1, L'\\');
                result += L'"';
                backslash_count = 0;
                break;
            default:
                result.append(backslash_count, L'\\');
                backslash_count = 0;
                result += arg[i];
            }
        }
        result.append(backslash_count * 2, L'\\');
        result += L'"';
        result += L' ';
    }

    if (!result.empty())
        result.pop_back(); // remove trailing space
    return result;
}

// source: https://blogs.msdn.microsoft.com/oldnewthing/20111216-00/?p=8873/
BOOL CreateProcessWithExplicitHandlesW(
    __in_opt     LPCWSTR lpApplicationName,
    __inout_opt  LPWSTR lpCommandLine,
    __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
    __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in         BOOL bInheritHandles,
    __in         DWORD dwCreationFlags,
    __in_opt     LPVOID lpEnvironment,
    __in_opt     LPCWSTR lpCurrentDirectory,
    __in         LPSTARTUPINFOW lpStartupInfo,
    __out        LPPROCESS_INFORMATION lpProcessInformation,
    // here is the new stuff
    __in         DWORD cHandlesToInherit,
    __in_ecount(cHandlesToInherit) HANDLE *rgHandlesToInherit)
{
    BOOL fInitialized = FALSE;
    SIZE_T size = 0;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList = nullptr;

    BOOL fSuccess = cHandlesToInherit < 0xFFFFFFFF / sizeof(HANDLE) &&
        lpStartupInfo->cb == sizeof(*lpStartupInfo);
    if (!fSuccess) {
        SetLastError(ERROR_INVALID_PARAMETER);
    }
    if (fSuccess) {
        fSuccess = InitializeProcThreadAttributeList(nullptr, 1, 0, &size) ||
            GetLastError() == ERROR_INSUFFICIENT_BUFFER;
    }
    if (fSuccess) {
        lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>
            (HeapAlloc(GetProcessHeap(), 0, size));
        fSuccess = lpAttributeList != nullptr;
    }
    if (fSuccess) {
        fSuccess = InitializeProcThreadAttributeList(lpAttributeList,
            1, 0, &size);
    }
    if (fSuccess) {
        fInitialized = TRUE;
        fSuccess = UpdateProcThreadAttribute(lpAttributeList,
            0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
            rgHandlesToInherit,
            cHandlesToInherit * sizeof(HANDLE), nullptr, nullptr);
    }
    if (fSuccess) {
        STARTUPINFOEXW info;
        ZeroMemory(&info, sizeof(info));
        info.StartupInfo = *lpStartupInfo;
        info.StartupInfo.cb = sizeof(info);
        info.lpAttributeList = lpAttributeList;
        fSuccess = CreateProcessW(lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags | EXTENDED_STARTUPINFO_PRESENT,
            lpEnvironment,
            lpCurrentDirectory,
            &info.StartupInfo,
            lpProcessInformation);
    }

    if (fInitialized) DeleteProcThreadAttributeList(lpAttributeList);
    if (lpAttributeList) HeapFree(GetProcessHeap(), 0, lpAttributeList);
    return fSuccess;
}

class inherit_helper
{
    fd_t handle;
public:
    explicit inherit_helper(fd_t handle)
        : handle(handle)
    {
        winapi_thrower(SetHandleInformation(handle, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT));
    }

    ~inherit_helper()
    {
        // Let's assume this won't fail. This is not a bad assumption, because
        // we get here only if a previous SetHandleInformation to the same handle succeeded.
        // Anyway, this class is used only on XP or below.
        SetHandleInformation(handle, HANDLE_FLAG_INHERIT, 0);
    }
};

struct wait_helper // working around missing [=, a = b] feature in C++11
{
    open_wrapper process_handle;
    open_wrapper thread_handle;

    int operator()() const
    {
        winapi_thrower(WaitForSingleObject(process_handle.get(), INFINITE)); // signaled?
        DWORD exitcode;
        winapi_thrower(GetExitCodeProcess(process_handle.get(), &exitcode));
        return int(exitcode);
    }
};

} // namespace

void command_native::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t>) const
{
    STARTUPINFOW startupinfo = {0};
    startupinfo.cb = sizeof(startupinfo);
    startupinfo.hStdInput = in;
    startupinfo.hStdOutput = out;
    startupinfo.hStdError = err;
    startupinfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processinfo = {nullptr};

    tstring_t cmdargs = convert_argv(get_argv());

    if(inherit_by_default())
    {
        fd_t inherited[] = {in, out, err};
        winapi_thrower(CreateProcessWithExplicitHandlesW(nullptr, &cmdargs[0], nullptr, nullptr, true, 0, nullptr, nullptr, &startupinfo, &processinfo, 3, inherited), p.string());
    }
    else
    {
        inherit_helper temp1{in};
        inherit_helper temp2{out};
        inherit_helper temp3{err};
        winapi_thrower(CreateProcessW(nullptr, &cmdargs[0], nullptr, nullptr, true, 0, nullptr, nullptr, &startupinfo, &processinfo), p.string());
    }

    open_wrapper proc{processinfo.hProcess};
    open_wrapper thr{processinfo.hThread};

    wait_helper helper{std::move(proc), std::move(thr)};
    result = std::async(std::launch::async, std::move(helper));
}

template<stdfd DESC>
command_redirect<DESC>::command_redirect(command c, fs::path p, bool append)
    : c(std::move(c))
    , p(std::move(p))
    , flags(int(append))
{ }

template<stdfd DESC>
void command_redirect<DESC>::start_run(fd_t in, fd_t out, fd_t err, std::vector<fd_t> unused_fds) const
{
    DWORD dwCreationDisposition = DESC == stdfd::in ? OPEN_EXISTING : (bool(flags) ? OPEN_ALWAYS      : CREATE_ALWAYS);
    DWORD dwDesiredAccess =       DESC == stdfd::in ? GENERIC_READ  : (bool(flags) ? FILE_APPEND_DATA : GENERIC_WRITE);

    SECURITY_ATTRIBUTES secattr = {0};
    secattr.nLength = sizeof(secattr);
    secattr.lpSecurityDescriptor = nullptr;
    secattr.bInheritHandle = inherit_by_default();

    fd = open_wrapper{CreateFileW(p.c_str(), dwDesiredAccess, 0, &secattr, dwCreationDisposition, 0, nullptr)};

    fd_t fds[int(stdfd::count)] = {in, out, err};
    fds[int(DESC)] = fd.get();
    c.start_run(fds[int(stdfd::in)], fds[int(stdfd::out)], fds[int(stdfd::err)], std::move(unused_fds));
}

template
class command_redirect<stdfd::in>;

template
class command_redirect<stdfd::out>;

template
class command_redirect<stdfd::err>;

} // namespace internal
} // namespace ccsh

#endif // _WIN32
