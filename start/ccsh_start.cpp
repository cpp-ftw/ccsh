#include <ccsh/ccsh.hpp>

using namespace ccsh;

int main(int argc, const char** argv)
{
    (void) argc;

    fs::path p = fs::current_path() / fs::path(argv[0]);
    p = p.parent_path();

    shell("cling", {"-std=c++14", 
                    "-L" + (p / "lib").string(),                // -Llib
                    "-l", "ccsh_lib",                           // -l ccsh_lib
                    "-l", (p / "start/clingrc.hpp").string(),   // -l start/clingrc.hpp
                    "-I" + (p / "include").string(),            // -Iinclude
                    "-I" + (p / "wrappers").string(),           // -Iwrappers
                    "--nologo"});
}
