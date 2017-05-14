#ifndef CCSH_COMMONS_HPP
#define CCSH_COMMONS_HPP

#define CCSH_NON_STANDARD_DOLLAR

#include <ccsh/ccsh.hpp>

#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>

#include <gtest/gtest.h>

using namespace ccsh::literals;

inline std::string ReadAllText(ccsh::fs::path const& filename)
{
    std::ifstream t(filename.c_str());
    return std::string(std::istreambuf_iterator<char>(t),
                       std::istreambuf_iterator<char>());
}

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) { return !std::isspace(c); }));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](char c) { return !std::isspace(c); }).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

inline ccsh::fs::path test_dir()
{
    static ccsh::fs::path result = ccsh::fs::path{__FILE__}.parent_path() / "temp"_p;
    static ccsh::fs::error_code ec;
    static bool unused = (ccsh::fs::create_directory(result, ec), false);
    (void)ec;
    (void)unused;
    return result;
}

#ifdef _WIN32
static std::string portable_eol = "\r\n";
#else
static std::string portable_eol = "\n";
#endif // _WIN32

#endif //CCSH_COMMONS_HPP
