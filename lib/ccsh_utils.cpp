#include <ccsh/ccsh_utils.hpp>
#include "ccsh_internals.hpp"

#include <cstring>
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

namespace {

// C++14 provide a mismatch algorithm with four iterator arguments(), but earlier
// standard libraries didn't, so provide this needed functionality.
inline std::pair<fs::path::iterator, fs::path::iterator> mismatch(fs::path::iterator it1, fs::path::iterator it1end,
                                                                  fs::path::iterator it2, fs::path::iterator it2end)
{
    for(; it1 != it1end && it2 != it2end && *it1 == *it2;)
    {
        ++it1;
        ++it2;
    }
    return std::make_pair(it1, it2);
}

fs::path lexically_relative(const fs::path& self, const fs::path& base)
{
    std::pair<fs::path::iterator, fs::path::iterator> mm
        = mismatch(self.begin(), self.end(), base.begin(), base.end());
    if(mm.first == self.begin() && mm.second == base.begin())
        return fs::path();
    if(mm.first == self.end() && mm.second == base.end())
        return fs::path(".");
    fs::path tmp;
    for(; mm.second != base.end(); ++mm.second)
        tmp /= fs::path(".");
    for(; mm.first != self.end(); ++mm.first)
        tmp /= *mm.first;
    return tmp;
}

fs::path relative(const fs::path& p, const fs::path& base, std::error_code& ec)
{
    fs::path wc_base(fs::canonical(base, ec));
    if(ec)
        return fs::path();
    fs::path wc_p(fs::canonical(p, ec));
    if(ec)
        return fs::path();
    return lexically_relative(wc_p, wc_base);
}

}

fs::path get_current_path_abbreviated()
{
    fs::path wd = get_current_path();
    fs::path home = get_home();
    std::error_code errcode;
    auto abb_wd = relative(wd, home, errcode);
    if(errcode)
        return wd;
    return fs::path("~") / abb_wd;
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
    stdc_thrower(try_set(name, value, override));
}

int env_var::try_set(std::string const& name, std::string const& value, bool override)
{
    return setenv(name.c_str(), value.c_str(), override);
}

stdc_error::stdc_error(int no)
    : std::runtime_error(strerror(errno))
    , error_number(no)
{}

stdc_error::stdc_error(int no, std::string const& msg)
    : std::runtime_error(msg.empty() ? strerror(no) : msg + ": " + strerror(no))
    , error_number(no)
{}

env_var::operator std::string() const
{
    const char* result = get(name);
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
