#include <iostream>
#include "ccsh.hpp"


int main()
{
    std::cout << ccsh::pwd() << std::endl;

//    std::cout << ccsh::command("cat", {"asd.txt"}).run() << std::endl;

    std::string f = "/tmp/bs.txt";

    auto c1 = ccsh::shell("ls", {"-lah"}) > f;
    c1->run();

    ccsh::shell("cat", {f})->run();

    auto c2 = ccsh::shell("ls", {"-lah"}) | ccsh::shell("cowsay");
    c2->run();

    ccsh::shell("echo", {"--- ls finished. ---"})->run();
//
//    auto c2 = ccsh::command("cowsay");


    return 0;
}

