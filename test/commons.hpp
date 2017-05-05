#ifndef CCSH_COMMONS_HPP
#define CCSH_COMMONS_HPP

#define CCSH_NON_STANDARD_DOLLAR

#include <ccsh/ccsh.hpp>

#include <fstream>
#include <string>

#include <gtest/gtest.h>

using namespace ccsh::literals;

inline std::string ReadAllText(ccsh::fs::path const& filename)
{
    std::ifstream t(filename.c_str());
    return std::string(std::istreambuf_iterator<char>(t),
                       std::istreambuf_iterator<char>());
}

static ccsh::fs::path test_dir = ccsh::fs::path{__FILE__}.parent_path();

#endif //CCSH_COMMONS_HPP
