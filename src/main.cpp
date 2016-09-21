#include <iostream>
#include "ccsh.hpp"


int main()
{
    std::cout << ccsh::pwd() << std::endl;

    std::cout << ccsh::shell("cat", {"/you/shall/not/path"}).run() << std::endl;

    std::string f = "/tmp/bs.txt";

    ccsh::command c1 = ccsh::shell("ls", {"-lah"}) > f;
    c1.run();

    ccsh::shell("cat", {f});

    ccsh::shell("ls", {"-lah"}) | ccsh::shell("cowsay");

    ccsh::shell("echo", {"--- ls finished. ---"});

    //auto c2 = ccsh::shell("cowsay");


    return 0;
}

