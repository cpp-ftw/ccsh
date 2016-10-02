#include <iostream>
#define CCSH_NON_STANDARD_DOLLAR
#include "ccsh.hpp"

// Not for real use, for syntax demonstration only.

class gcc_t : public ccsh::command_native, public ccsh::command_builder_base
{
public:

    gcc_t(std::vector<std::string> const& args)
        : command_native("gcc", args)
    { }

    ccsh::command_holder<gcc_t>& o(ccsh::fs::path const& p)
    {
        args.push_back(p.string());
        return static_cast<ccsh::command_holder<gcc_t>&>(*this);
    }
};

using gcc = ccsh::command_holder<gcc_t>;

void test1()
{
    ccsh::command c1 = gcc({}).o("");
    auto c2 = c1;
    ccsh::command_builder<gcc> x = gcc({}).o("");

    x.o("asd.o");
}

void test2()
{
    using namespace ccsh;
    std::string user = $("USER");
    std::cout << user << " " << user.length() << std::endl;
    $("BAFFER") = "some text";
    std::cout << $("BAFFER") << std::endl;

    shell("echo", {$(shell("ls"))});
}

using namespace ccsh::literals;

int main()
{
    test2();
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

