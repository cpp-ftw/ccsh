#ifndef CCSH_GCC_HPP
#define CCSH_GCC_HPP

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {

class gcc_t : public wrappers::options_paths<gcc_t>
{
    using base = wrappers::options_paths<gcc_t>;
    friend base;
    static constexpr const char* name() { return "g++"; }

public:
    using base::base;

    CCSH_WRAPPER_ARG1_N(gcc_t, input, "", fs::path const&, arg.string())

    CCSH_WRAPPER_ARG1_S(gcc_t, o, "-o", fs::path const&, arg.string())
    CCSH_WRAPPER_ARG1_S(gcc_t, output, "-o", fs::path const&, arg.string())
    CCSH_WRAPPER_ARG0(gcc_t, c, "-c")

    CCSH_WRAPPER_ARG1_N(gcc_t, D, "-D", std::string, std::move(arg))
    CCSH_WRAPPER_ARG2_NE(gcc_t, D, "-D", std::string, std::move(arg1), std::string, std::move(arg2))
    CCSH_WRAPPER_ARG1_N(gcc_t, l, "-l", std::string, std::move(arg))
    CCSH_WRAPPER_ARG1_N(gcc_t, I, "-I", fs::path const&, arg.string())
    CCSH_WRAPPER_ARG1_N(gcc_t, L, "-L", fs::path const&, arg.string())
    CCSH_WRAPPER_ARG1_N(gcc_t, rpath, "-Wl,-rpath,", fs::path const&, arg.string())

    CCSH_WRAPPER_ARG1_E(gcc_t, std, "-std", std::string const&, arg)

    CCSH_WRAPPER_ARG0(gcc_t, PIC, "-fPIC")
    CCSH_WRAPPER_ARG0(gcc_t, shared, "-shared")
    CCSH_WRAPPER_ARG0(gcc_t, MM, "-MM")
};

using gcc = command_holder<gcc_t>;

}

#endif //CCSH_GCC_HPP
