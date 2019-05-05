#include "../ccsh_internals.hpp"
#include <ccsh/builtins/cd.hpp>
#include <ccsh/ccsh_fdstream.hpp>
#include <ccsh/ccsh_operators.hpp>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#define chdir _wchdir
#endif

namespace {

using ccsh::fs::path;

/* Do the work of changing to the directory NEWDIR. Handle symbolic
   link following, etc. This function *must* return with
   the_current_working_directory either set to NULL (in which case
   getcwd() will eventually be called), or set to a string corresponding
   to the working directory. Return 0 on success, 1 on failure. */
int change_to_directory(path newdir, bool follow_symlinks)
{
    //char* t = make_absolute(newdir, tcwd);
    path tcwd = ccsh::get_current_path() / newdir;

    // TDIR is either
    // - the canonicalized absolute pathname of NEWDIR (follow_symlinks)
    // - or the absolute physical pathname of NEWDIR (!follow_symlinks).
    ccsh::fs::error_code ec;
    path tdir = follow_symlinks
                ? ccsh::fs::self_lexically_normal(tcwd)
                : ccsh::fs::canonical(tcwd, ec);

    // Use the canonicalized version of NEWDIR, or, if canonicalization
    // failed, use the non-canonical form.
    bool canon_failed = false;
    if (ec)
    {
        tdir = tcwd;
        canon_failed = true;
    }

    // In POSIX mode, if we're resolving symlinks logically and sh_canonpath
    // returns NULL (because it checks the path, it will return NULL if the
    // resolved path doesn't exist), fail immediately.
    if (follow_symlinks && canon_failed)
    {
        if (errno != ENOENT)
            errno = ENOTDIR;
        return EXIT_FAILURE;
    }

    // If the chdir succeeds, update the_current_working_directory.
    if (chdir((follow_symlinks ? tdir : newdir).c_str()) == 0)
        return EXIT_SUCCESS;

    // We failed to change to the appropriate directory name. If we tried what
    // the user passed (follow_symlinks), punt now.
    if (!follow_symlinks)
        return EXIT_FAILURE;

    // We're not in physical mode (!follow_symlinks), but we failed to change to the
    // canonicalized directory name (TDIR).  Try what the user passed verbatim.
    int err = errno;
    if (chdir(newdir.c_str()) == 0)
        return EXIT_SUCCESS;

    errno = err;
    return EXIT_FAILURE;
}


int bindpwd(bool follow_symlinks, bool eflag)
{
    path tcwd = ccsh::get_current_path();
    ccsh::fs::error_code ec;

    if (follow_symlinks)
        tcwd = ccsh::fs::canonical(tcwd, ec);

    if (ec)
        return eflag ? EXIT_FAILURE : EXIT_SUCCESS;

    if (ccsh::env_var::try_set("OLDPWD", ccsh::env_var::get("PWD")) < 0)
        return EXIT_FAILURE;

    if (ccsh::env_var::try_set("PWD", tcwd.string()) < 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

const std::string help_message = R"delim(cd: cd [-L|[-P [-e]]] [dir]
    Change the shell working directory.

    Change the current directory to DIR.  The default DIR is the value of the
    HOME shell variable.

    The variable CDPATH defines the search path for the directory containing
    DIR.  Alternative directory names in CDPATH are separated by a colon (:).
    A null directory name is the same as the current directory.  If DIR begins
    with a slash (/), then CDPATH is not used.

    Options:
      -L	force symbolic links to be followed: resolve symbolic
    		links in DIR after processing instances of `..'
      -P	use the physical directory structure without following
    		symbolic links: resolve symbolic links in DIR before
    		processing instances of `..'
      -e	if the -P option is supplied, and the current working
    		directory cannot be determined successfully, exit with
    		a non-zero status

    The default is to follow symbolic links, as if `-L' were specified.
    `..' is processed by removing the immediately previous pathname component
    back to a slash or the beginning of DIR.

    Exit Status:
    Returns 0 if the directory is changed, and if $PWD is set successfully when
    -P is used; non-zero otherwise.
)delim";

} // namespace

namespace ccsh {

int cd_t::runx(fd_t, fd_t out_fd, fd_t err_fd) const
{
    ofdstream out{out_fd};
    ofdstream err{err_fd};

    if (helpflag)
    {
        out << help_message << std::flush;
        return EXIT_SUCCESS;
    }

    bool printflag = false;
    bool eflag = this->eflag;
    if (eflag && follow_symlinks)
        eflag = false;

    std::string dir = p.string();
    const char* dirname = dir.c_str();

    if (dirname == std::string("-"))
    {
        dirname = env_var::try_get("OLDPWD");
        if (dirname == nullptr)
        {
            err << "OLDPWD not set" << std::endl;
            return EXIT_FAILURE;
        }
        printflag = true;
    }
    else
    {
        std::string cdpath = dollar("CDPATH");

        char* saveptr = nullptr;
        char* token = internal::strtok_compat(&cdpath[0], ":", &saveptr);

        while (token != nullptr)
        {
            fs::path temp = fs::path{token} / fs::path{dirname};
            if (change_to_directory(temp, follow_symlinks) == EXIT_SUCCESS)
            {
                /* POSIX.2 says that if a nonempty directory from CDPATH
                is used to find the directory to change to, the new
                directory name is echoed to stdout, whether or not
                the shell is interactive. */
                if (token[0] != '\0')
                {
                    if (follow_symlinks)
                        out << get_current_path().string() << std::endl;
                    else
                        out << temp.string() << std::endl;
                }
                return bindpwd(follow_symlinks, eflag);
            }
            token = internal::strtok_compat(nullptr, ":", &saveptr);
        }
    }

    /* When we get here, DIRNAME is the directory to change to.  If we
       chdir successfully, just return. */
    if (change_to_directory(dirname, follow_symlinks) == EXIT_SUCCESS)
    {
        if (printflag)
            out << dirname << std::endl;

        return bindpwd(follow_symlinks, eflag);
    }

    err << dirname << ": " << stdc_error::strerror(errno) << std::endl;
    return EXIT_FAILURE;
}

} // namespace ccsh

