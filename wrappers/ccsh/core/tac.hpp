#ifndef CCSH_CORE_TAC_HPP_INCLUDED
#define CCSH_CORE_TAC_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {
namespace core {

class tac_t : public wrappers::options_paths<tac_t>
{
    using base = wrappers::options_paths<tac_t>;
    static constexpr const char * name() { return "tac"; }
    friend base;    

public:

    using base::base;

    CCSH_WRAPPER_ARG0(tac_t, b, "-b")
    CCSH_WRAPPER_ARG0(tac_t, before, "--before")

    CCSH_WRAPPER_ARG0(tac_t, r, "-r")
    CCSH_WRAPPER_ARG0(tac_t, regex, "--regex")

    CCSH_WRAPPER_ARG1(tac_t, s, "-s", std::string, std::move)
    CCSH_WRAPPER_ARG1_FWD(tac_t, separator, s, std::string)

};

using tac = command_holder<tac_t>;


}
}

#endif // CCSH_CORE_TAC_HPP_INCLUDED
