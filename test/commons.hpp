#ifndef CCSH_COMMONS_HPP
#define CCSH_COMMONS_HPP

#define CCSH_NON_STANDARD_DOLLAR

#include <ccsh/ccsh.hpp>

#include <fstream>
#include <string>
#include <algorithm>

#include <gtest/gtest.h>

using namespace ccsh::literals;

inline std::string ReadAllText(ccsh::fs::path const& filename)
{
    std::ifstream t(filename.c_str());
    return std::string(std::istreambuf_iterator<char>(t),
                       std::istreambuf_iterator<char>());
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

#endif //CCSH_COMMONS_HPP
