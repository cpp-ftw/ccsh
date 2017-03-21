#ifndef CCSH_CORE_WC_HPP_INCLUDED
#define CCSH_CORE_WC_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {
namespace core {


class wc_t : public wrappers::options_paths<wc_t>
{
    using base = wrappers::options_paths<wc_t>;
    friend base;
    static constexpr const char * name() { return "wc"; }

public:

    using base::base;

    CCSH_WRAPPER_ARG0(wc_t, c, "-c")
    CCSH_WRAPPER_ARG0(wc_t, bytes, "--bytes")

    CCSH_WRAPPER_ARG0(wc_t, m, "-m")
    CCSH_WRAPPER_ARG0(wc_t, chars, "--chars")

    CCSH_WRAPPER_ARG0(wc_t, w, "-w")
    CCSH_WRAPPER_ARG0(wc_t, words, "--words")

    CCSH_WRAPPER_ARG0(wc_t, l, "-l")
    CCSH_WRAPPER_ARG0(wc_t, lines, "--lines")

    CCSH_WRAPPER_ARG1(wc_t, L, "-L", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(wc_t, max_line_length, L, unsigned)

    // TODO: files0_from
    
};

using wc = command_holder<wc_t>;

}
}

#endif // CCSH_CORE_WC_HPP_INCLUDED
