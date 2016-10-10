#include "ccsh.hpp"

int main()
{
	// cling -std=c++14  -l ccsh_lib -l "src/clingrc.hpp" --nologo
	ccsh::shell("cling", {"-std=c++14", "-l", "ccsh_lib", "-l", "src/clingrc.hpp", "--nologo"});
}
