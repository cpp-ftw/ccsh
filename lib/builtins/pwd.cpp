#include <ccsh/builtins/pwd.hpp>
#include <ccsh/ccsh_fdstream.hpp>
#include "../ccsh_internals.hpp"

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

int pwd_t::runx(int, int out_fd, int) const
{
    ofdstream out{out_fd};

    if (helpflag)
    {
        out << help_message << std::flush;
        return EXIT_SUCCESS;
    }

    fs::error_code ec;
    fs::path tcwd = fs::current_path(ec);
    if (verbatim_pwd)
        tcwd = fs::canonical(tcwd);

    if (ec)
        return EXIT_FAILURE;

    out << tcwd.string() << std::endl;

    /* This is dumb but posix-mandated. */
    if (verbatim_pwd)
        if (env_var::try_set("PWD", tcwd.string()) < 0)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
}
