#include <ccsh/ccsh_utils.hpp>
#include "ccsh_internals.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <glob.h>

#include <memory>

namespace ccsh {

namespace fs {

namespace {

void expand_helper(path const& p, std::vector<path>& result)
{
    glob_t globbuf;
    glob(p.string().c_str(), GLOB_NOCHECK | GLOB_TILDE_CHECK, NULL, &globbuf);

    for(std::size_t i = 0; i < globbuf.gl_pathc; ++i)
        result.push_back(globbuf.gl_pathv[i]);

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
    for(path const& p : paths)
        expand_helper(p, result);
    return result;
}


}

fs::path get_home()
{
    struct passwd pwd;
    struct passwd* result;

    long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(bufsize == -1)
        bufsize = 16384;

    std::unique_ptr<char[]> buf{new char[bufsize]};

    if(getpwuid_r(getuid(), &pwd, buf.get(), bufsize, &result) != 0 ||
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
    if(wd == get_home())
        return "~";
    return wd.filename();
}

fs::path get_current_path_abbreviated()
{
    fs::path wd = get_current_path();
    fs::path home = get_home();
    boost::system::error_code errcode;
    auto abb_wd = fs::relative(wd, home, errcode);
    if(errcode)
        return wd;
    return fs::path("~")/abb_wd;
}

std::string get_hostname()
{
    char buf[HOST_NAME_MAX + 1];
    stdc_thrower(gethostname(buf, sizeof(buf)));
    return buf;
}

std::string get_short_hostname()
{
    std::string hn = get_hostname();
    auto pos = hn.find('.');
    if(pos == std::string::npos)
        return hn;
    return hn.substr(0, pos);
}

std::string get_shell_name()
{
    return "ccsh";
}


bool is_user_possibly_elevated()
{
    uid_t uid = getuid();
    uid_t euid = geteuid();

    return uid <= 0 || uid != euid;
}

env_var::operator std::string() const
{
    const char* result = getenv(name.c_str());
    return result == nullptr ? "" : result;
}

env_var& env_var::operator=(std::string const& str)
{
    stdc_thrower(setenv(name.c_str(), str.c_str(), true));
    return *this;
}

namespace internal {

void open_traits::dtor_func(int fd) noexcept
{
    if(fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
        close_fd(fd);
}

static_assert(int(stdfd::in) == STDIN_FILENO, "Error in stdfd enum.");
static_assert(int(stdfd::out) == STDOUT_FILENO, "Error in stdfd enum.");
static_assert(int(stdfd::err) == STDERR_FILENO, "Error in stdfd enum.");

}

} // namespace ccsh
