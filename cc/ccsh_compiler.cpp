#include <iostream>

#define CCSH_NON_STANDARD_DOLLAR

#include <ccsh/ccsh.hpp>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

#include <unistd.h>

using namespace ccsh;

bool starts_with_dash(std::string const& str)
{
    return str.length() > 0 && str[0] == '-';
}

bool not_starts_with_dash(std::string const& str)
{
    return !starts_with_dash(str);
}

std::unordered_set<std::string> gcc_options_with_args = {
    "-x",
    "-o",
    "-aux-info",
    "--param",
    "-D",
    "-U",
    "-include",
    "-imacros",
    "-MF",
    "-MT",
    "-MQ",
    "-A",
    "-Xpreprocessor",
    "-Xassembler",
    "-l",
    "-T",
    "-Xlinker",
    "-u",
    "-z",
    "-I",
    "-iquote",
    "-isystem",
    "-idirafter",
    "-iprefix",
    "-iwithprefix",
    "-iwithprefixbefore",
    "-isysroot",
    "-imultilib",
    "-L",
};

std::vector<std::string>::const_iterator find_file(std::vector<std::string> const& options)
{
    for (auto it = options.begin(); it != options.end(); ++it)
    {
        if ((*it)[0] == '-')
        {
            if (gcc_options_with_args.count(*it) != 0)
                ++it;
        }
        else if ((*it)[0] != '@')
            return it;
    }
    return options.end();
}

void print_usage()
{
    std::cout << "Usage: ccshc [gcc-options] file [file-options]" << std::endl;
    std::cout << "Note: it can process only ONE file!" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    bool compile_only = false;
    std::string compile_only_str = "-c";

    std::vector<std::string> args;
    args.reserve(std::size_t(argc));
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i] != compile_only_str)
            args.emplace_back(argv[i]);
        else
            compile_only = true;
    }

    auto file_it = find_file(args);
    if (file_it == args.end())
    {
        print_usage();
        return 1;
    }

    std::vector<std::string> gcc_args;
    gcc_args.reserve(file_it - args.begin() + 5);
    gcc_args.emplace_back("-x");
    gcc_args.emplace_back("c++");
    gcc_args.emplace_back("-l");
    gcc_args.emplace_back("ccsh_lib");

    for (auto it = args.begin(); it != file_it; ++it)
    {
        if (*it == compile_only_str)
            compile_only = true;
        else
            gcc_args.push_back(*it);
    }

    fs::path temp = "/tmp/ccshc";
    fs::create_directory(temp);

    temp /= generate_filename();

    gcc_args.emplace_back("-o");
    gcc_args.push_back(temp.string());
    gcc_args.emplace_back("-");

    std::string cxx = ccsh::$("CXX");
    if (cxx.empty())
        cxx = "/usr/bin/c++";

    auto const& file = *file_it;
    auto shebang_remover = [&](int, int out_fd, int)
    {
        ccsh::ofdstream out{out_fd};
        try
        {
            std::ifstream ifs(file);
            std::string str;

            while (std::getline(ifs, str))
            {
                if (str.length() >= 2 && str[0] == '#' && str[1] == '!')
                    continue;

                out << str << std::endl;
            }
        }
        catch (std::exception const& x)
        {
            std::cerr << file << ": " << x.what() << std::endl;
            return 1;
        }
        return 0;
    };

    command c = shebang_remover | shell(cxx, gcc_args);
    int result = c.run();
    if (result == 0 && !compile_only)
    {
        std::vector<std::string> program_args;
        std::copy(file_it + 1, args.cend(), std::back_inserter(program_args));
        result = shell(temp, program_args).run();
    }
    if (result != 0)
    {
        std::cerr << "While executing " << file << std::endl;
    }

    std::remove(temp.c_str());

    return result;
}

