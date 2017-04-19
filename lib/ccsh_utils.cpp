#include <ccsh/ccsh_utils.hpp>
#include "ccsh_internals.hpp"

#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <climits>
#include <pwd.h>
#include <glob.h>

#include <memory>

namespace ccsh {

namespace fs {

namespace {

void expand_helper(path const& p, std::vector<path>& result)
{
    glob_t globbuf;
    glob(p.string().c_str(), GLOB_NOCHECK | GLOB_TILDE_CHECK, nullptr, &globbuf);

    for (std::size_t i = 0; i < globbuf.gl_pathc; ++i)
        result.emplace_back(globbuf.gl_pathv[i]);

    globfree(&globbuf);
}

}

std::vector<path> expand(path const& p)
{
    std::vector<path> result;
    expand_helper(p, result);
    return result;
}

std::vector<path> expand(std::vector<path> const& paths)
{
    std::vector<path> result;
    for (path const& p : paths)
        expand_helper(p, result);
    return result;
}

}

fs::path get_home()
{
    struct passwd pwd;
    struct passwd* result;

    long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)
        bufsize = 16384;

    std::unique_ptr<char[]> buf{new char[bufsize]};

    if (getpwuid_r(getuid(), &pwd, buf.get(), std::size_t(bufsize), &result) != 0 ||
        result == nullptr ||
        result->pw_dir == nullptr)
    {
        throw stdc_error();
    }

    return fs::path{result->pw_dir};
}

fs::path get_current_path()
{
    return fs::current_path();
}

fs::path get_current_directory()
{
    fs::path wd = get_current_path();
    if (wd == get_home())
        return "~";
    return wd.filename();
}

fs::path get_current_path_abbreviated()
{
    fs::path wd = get_current_path();
    fs::path home = get_home();
    fs::error_code errcode;
    auto abb_wd = fs::relative(wd, home, errcode);
    if (errcode)
        return wd;
    return fs::path("~") / abb_wd;
}

std::string get_hostname()
{
    char buf[HOST_NAME_MAX + 1];
    internal::stdc_thrower(gethostname(buf, sizeof(buf)));
    return buf;
}

std::string get_short_hostname()
{
    std::string hn = get_hostname();
    auto pos = hn.find('.');
    if (pos == std::string::npos)
        return hn;
    return hn.substr(0, pos);
}

std::string get_shell_name()
{
    return "ccsh";
}

std::string get_ttyname()
{
    fs::path result = ttyname(fileno(stdin));
    return result.string();
}


bool is_user_possibly_elevated()
{
    uid_t uid = getuid();
    uid_t euid = geteuid();

    return uid <= 0 || uid != euid;
}

const char* env_var::get(std::string const& name)
{
    return getenv(name.c_str());
}

void env_var::set(std::string const& name, std::string const& value, bool override)
{
    internal::stdc_thrower(try_set(name, value, override));
}

int env_var::try_set(std::string const& name, std::string const& value, bool override)
{
    return setenv(name.c_str(), value.c_str(), int(override));
}

stdc_error::stdc_error(int no)
    : std::runtime_error(strerror(errno))
    , error_number(no)
{ }

stdc_error::stdc_error(int no, std::string const& msg)
    : std::runtime_error(msg.empty() ? strerror(no) : msg + ": " + strerror(no))
    , error_number(no)
{ }

env_var::operator std::string() const
{
    const char* result = get(name);
    return result == nullptr ? "" : result;
}

env_var& env_var::operator=(std::string const& str)
{
    internal::stdc_thrower(setenv(name.c_str(), str.c_str(), int(true)));
    return *this;
}

namespace internal {

void open_traits::dtor_func(int fd) noexcept
{
    if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
        close_fd(fd);
}

static_assert(int(stdfd::in) == STDIN_FILENO, "Error in stdfd enum.");
static_assert(int(stdfd::out) == STDOUT_FILENO, "Error in stdfd enum.");
static_assert(int(stdfd::err) == STDERR_FILENO, "Error in stdfd enum.");

}

} // namespace ccsh
