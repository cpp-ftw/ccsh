#include <iostream>
#include "ccsh.hpp"

using namespace ccsh::literals;

int main()
{
    ccsh::shell("la");
    std::cout << ccsh::pwd() << std::endl;
    ccsh::shell("la") | ccsh::shell("cowsay"); // no cow should appear

    //std::cout << ccsh::shell("cat", {"/you/shall/not/path"}).run() << std::endl;

    ccsh::fs::path f = "/tmp/bs.txt"_p;

    ccsh::command c1 = ccsh::shell("ls", {"-lh"}) > f;
    c1.run();

    ccsh::shell("cat", {f.string()});

    ccsh::shell("ls", {"-lh"}) | ccsh::shell("cowsay") | ccsh::shell("lolcat");

    std::string str;
    ccsh::shell("echo", {"--- ls finished. ---"}) > str;
    std::cout << str.size() << std::endl;
    std::cout << str << std::endl;

    ccsh::shell("lolcat") < str;

    std::vector<std::string> vec;
    ccsh::shell("ls", {"-1"}) > vec;

    for(auto const& s : vec)
        std::cout << s << std::endl;

    // Not compiling, as intended...
    //auto c2 = ccsh::shell("cowsay");


    return 0;
}

