#include <iostream>

#define CCSH_NON_STANDARD_DOLLAR

#include <ccsh/ccsh.hpp>

#include <ccsh/core.hpp>

#include <ccsh/ccsh_tty.hpp>

using namespace ccsh::literals;

void test_winapi()
{
    std::cout << ccsh::get_home().string() << std::endl;
    ccsh::shell("help") > "bs.txt"_p;
    ccsh::shell("help") | ccsh::shell("findstr", {"x*"});
}

void test0()
{
    std::cout << ccsh::get_home().string() << std::endl;
    ccsh::shell("la");

    // ccsh::shell("help");
    // ccsh::shell("dir").run();
    ccsh::shell("help") > "asd.txt"_p;
    // (ccsh::shell("help") | ccsh::shell("more")).run();

    ccsh::shell("la") | ccsh::shell("cowsay"); // no cow should appear

    ccsh::fs::path f = "/tmp/bs.txt"_p;

    ccsh::shell("ls", {"-lh"}) > f;

    ccsh::shell("cat", {f.string()});

    ccsh::shell("ls") | ccsh::shell("cat");

    ccsh::shell("ls", {"-lh"}) | ccsh::shell("cowsay") | ccsh::shell("lolcat");

    std::string str;
    ccsh::shell("echo", {"--- ls finished. ---"}) > str;
    std::cout << str.size() << std::endl;
    std::cout << str << std::endl;

    ccsh::shell("lolcat") < str;

    std::vector<std::string> vec;
    ccsh::shell("ls", {"-1"}) > vec;

    for (auto const& s : vec)
        std::cout << s << std::endl;

    // Not compiling, as intended...
    //auto c2 = ccsh::shell("cowsay");
}

void test1()
{
    using namespace ccsh;
    ccsh::command c1 = cat().e();
    // FIXME: FUCKING RETARDED MSVC
    // ccsh::command_builder<cat> x = cat().n();
    // x.T();
}

void test2()
{
    using namespace ccsh;
    std::string user = $("USER");
    std::cout << user << " " << user.length() << std::endl;
    $("BAFFER") = "some text";
    std::cout << $("BAFFER") << std::endl;

    // shell("echo", {$(shell("ls"))});
}

void test3()
{
    using namespace ccsh;
    ls().l().color(ls::auto_);
    ls("CMake*").l();
}

void test4()
{
    using namespace ccsh;

    ls(), ls();

    cat(__FILE__);
}

#ifndef _WIN32
void test5()
{
    ccsh::source("./vars");
    std::cout << ccsh::$("EXAMPLE_ENV_VAR1") << std::endl;
    std::cout << ccsh::$("EXAMPLE_ENV_VAR2") << std::endl;
}
#endif

int main()
{
    test_winapi();
    /*test0();
    test1();
    test2();
    test3();
    test4();
    test5();*/

    return 0;
}

