#include <ccsh/builtins/cd.hpp>
#include <ccsh/ccsh_operators.hpp>
#include "../ccsh_internals.hpp"
#include <cstring>
#include <cstdlib>

namespace {

using ccsh::fs::path;

const path dot_path{"."};
constexpr char dot{'.'};
constexpr char slash{'.'};

path normalize(path lhs)
{
    if(lhs.empty())
        return lhs;

    path temp;
    path::iterator start(lhs.begin());
    path::iterator last(lhs.end());
    path::iterator stop(last--);
    for(path::iterator itr(start); itr != stop; ++itr)
    {
        // ignore "." except at start and last
        if(itr->native().size() == 1
           && (itr->native())[0] == dot
           && itr != start
           && itr != last)
            continue;

        // ignore a name and following ".."
        if(!temp.empty()
           && itr->native().size() == 2
           && (itr->native())[0] == dot
           && (itr->native())[1] == dot) // dot dot
        {
            path::string_type lf(temp.filename().native());
            if(lf.size() > 0
               && (lf.size() != 1
                   || (lf[0] != dot
                       && lf[0] != slash))
               && (lf.size() != 2
                   || (lf[0] != dot
                       && lf[1] != dot
                   )
               )
                )
            {
                temp.remove_filename();
                //// if not root directory, must also remove "/" if any
                //if (temp.native().size() > 0
                //  && temp.native()[temp.native().size()-1]
                //    == separator)
                //{
                //  string_type::size_type rds(
                //    root_directory_start(temp.native(), temp.native().size()));
                //  if (rds == string_type::npos
                //    || rds != temp.native().size()-1)
                //  {
                //    temp.m_pathname.erase(temp.native().size()-1);
                //  }
                //}

                path::iterator next(itr);
                if(temp.empty() && ++next != stop
                   && next == last && *last == dot_path)
                {
                    temp /= dot_path;
                }
                continue;
            }
        }

        temp /= *itr;
    };

    if(temp.empty())
        temp /= dot_path;
    return temp;
}

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
    std::error_code ec;
    path tdir = follow_symlinks
                          ? normalize(tcwd)
                          : ccsh::fs::canonical(tcwd, ec);

    // Use the canonicalized version of NEWDIR, or, if canonicalization
    // failed, use the non-canonical form.
    bool canon_failed = 0;
    if(ec)
    {
        tdir = tcwd;
        canon_failed = 1;
    }

    // In POSIX mode, if we're resolving symlinks logically and sh_canonpath
    // returns NULL (because it checks the path, it will return NULL if the
    // resolved path doesn't exist), fail immediately.
    if(follow_symlinks && canon_failed)
    {
        if(errno != ENOENT)
            errno = ENOTDIR;
        return EXIT_FAILURE;
    }

    // If the chdir succeeds, update the_current_working_directory.
    if(chdir((follow_symlinks ? tdir : newdir).c_str()) == 0)
        return EXIT_SUCCESS;

    // We failed to change to the appropriate directory name. If we tried what
    // the user passed (follow_symlinks), punt now.
    if(!follow_symlinks)
        return EXIT_FAILURE;

    // We're not in physical mode (!follow_symlinks), but we failed to change to the
    // canonicalized directory name (TDIR).  Try what the user passed verbatim.
    int err = errno;
    if(chdir(newdir.c_str()) == 0)
        return EXIT_SUCCESS;
    else
        errno = err;
    return EXIT_FAILURE;
}


int bindpwd(bool follow_symlinks, bool eflag)
{
    path tcwd = ccsh::get_current_path();
    std::error_code ec;

    if(follow_symlinks)
        tcwd = ccsh::fs::canonical(tcwd, ec);

    if(ec)
        return eflag ? EXIT_FAILURE : EXIT_SUCCESS;

    if(ccsh::env_var::try_set("OLDPWD", ccsh::env_var::get("PWD")) < 0)
        return EXIT_FAILURE;

    if(ccsh::env_var::try_set("PWD", tcwd.string()) < 0)
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

int cd_t::runx(int, int out, int err) const
{
    if(helpflag)
    {
        write(out, help_message.c_str(), help_message.size() + 1);
        return EXIT_SUCCESS;
    }

    bool printflag = false;
    bool eflag = this->eflag;
    if(eflag && follow_symlinks)
        eflag = 0;

    const char* dirname = p.c_str();

    if(dirname == std::string("-"))
    {
        dirname = env_var::get("OLDPWD");
        if(dirname == nullptr)
        {
            std::string output = "OLDPWD not set\n";
            write(err, output.c_str(), output.size() + 1);

            return EXIT_FAILURE;
        }
        printflag = true;
    }
    else
    {
        std::string cdpath = dollar("CDPATH");

        char* saveptr = nullptr;
        char* token = strtok_r(&cdpath[0], ":", &saveptr);

        while(token != nullptr)
        {
            fs::path temp = fs::path{token} / fs::path{dirname};
            if(change_to_directory(temp, follow_symlinks) == EXIT_SUCCESS)
            {
                /* POSIX.2 says that if a nonempty directory from CDPATH
                is used to find the directory to change to, the new
                directory name is echoed to stdout, whether or not
                the shell is interactive. */
                if(token[0] != '\0')
                {
                    std::string output;
                    if(follow_symlinks)
                        output = get_current_path().string() + "\n";
                    else
                        output = temp.string() + "\n";

                    write(out, output.c_str(), output.size() + 1);
                }
                return bindpwd(follow_symlinks, eflag);
            }
            token = strtok_r(nullptr, ":", &saveptr);
        }
    }

    /* When we get here, DIRNAME is the directory to change to.  If we
       chdir successfully, just return. */
    if(change_to_directory(dirname, follow_symlinks) == EXIT_SUCCESS)
    {
        if(printflag)
        {
            std::string output = std::string(dirname) + "\n";
            write(out, output.c_str(), output.size() + 1);
        }
        return bindpwd(follow_symlinks, eflag);
    }

    std::string output = std::string(dirname) + ": " + strerror(errno) + "\n";
    write(err, output.c_str(), output.size() + 1);
    return EXIT_FAILURE;
}


}

