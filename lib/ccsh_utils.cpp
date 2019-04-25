#include "ccsh_internals.hpp"
#include <ccsh/ccsh_utils.hpp>

#include <climits>
#include <cstring>

#include <memory>
#include <random>
#include <algorithm>

#include "cygwin/glob.h"

namespace ccsh {

namespace fs {

namespace {

void expand_helper(path const& p, std::vector<path>& result)
{
    glob_t globbuf;
    glob(p.string().c_str(), GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf);

    for(std::size_t i = 0; i < globbuf.gl_pathc; ++i)
        result.push_back(globbuf.gl_pathv[i]);

    globfree(&globbuf);
}


} // namespace

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

} // namespace fs

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
    if(wd == home)
        return fs::path("~");
    fs::error_code errcode;
    auto abb_wd = fs::relative(wd, home, errcode);
    if (errcode)
        return wd;
    return fs::path("~") / abb_wd;
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

fs::path generate_filename()
{
    const std::string VALID_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    constexpr int RANDOM_SIZE = 36;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, int(VALID_CHARS.size() - 1));
    std::string your_random_string;
    std::generate_n(std::back_inserter(your_random_string), RANDOM_SIZE, [&]() -> char
    {
        return VALID_CHARS[distribution(generator)];
    });
    return fs::path{your_random_string};
}

stdc_error::stdc_error(int no)
    : shell_error(stdc_error::strerror(errno))
    , error_number(no)
{ }

stdc_error::stdc_error(int no, std::string const& msg)
    : shell_error(msg.empty() ? stdc_error::strerror(no) : msg + ": " + stdc_error::strerror(no))
    , error_number(no)
{ }

env_var::operator std::string() const
{
    const char* result = try_get(name);
    return result == nullptr ? "" : result;
}

env_var& env_var::operator=(std::string const& str)
{
    set(name, str);
    return *this;
}

} // namespace ccsh
