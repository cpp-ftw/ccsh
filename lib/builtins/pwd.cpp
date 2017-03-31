#include <ccsh/builtins/pwd.hpp>
#include "../ccsh_internals.hpp"
#include <cstring>
#include <iostream>
#include <cstdlib>

namespace {

const std::string help_message = R"delim(pwd: pwd [-LP]
    Print the name of the current working directory.

    Options:
      -L	print the value of $PWD if it names the current working
    		directory
      -P	print the physical directory, without any symbolic links

    By default, `pwd' behaves as if `-L' were specified.

    Exit Status:
    Returns 0 unless an invalid option is given or the current directory
    cannot be read.
)delim";

} // namespace

namespace ccsh {

int pwd_t::runx(int, int out, int) const
{
    if(helpflag)
    {
        write(out, help_message.c_str(), help_message.size() + 1);
        return EXIT_SUCCESS;
    }

    fs::error_code ec;
    fs::path tcwd = fs::current_path(ec);
    if(verbatim_pwd)
        tcwd = fs::canonical(tcwd);

    if(ec)
        return EXIT_FAILURE;

    std::string str = tcwd.string() + "\n";
    write(out, str.c_str(), str.size() + 1);

    /* This is dumb but posix-mandated. */
    if(verbatim_pwd)
        if(env_var::try_set("PWD", tcwd.string()) < 0)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

}
