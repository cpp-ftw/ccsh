#ifndef CCSH_CORE_HEAD_HPP_INCLUDED
#define CCSH_CORE_HEAD_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>
#include <ccsh/ccsh_ratio.hpp>

namespace ccsh {
namespace core {


class head_t : public wrappers::options_paths<head_t>
{
    using base = wrappers::options_paths<head_t>;
    friend base;
    static constexpr const char * name() { return "head"; }

public:

    using base::base;

    CCSH_WRAPPER_ARG0(head_t, q, "-q")
    CCSH_WRAPPER_ARG0(head_t, quiet, "--quiet")
    CCSH_WRAPPER_ARG0(head_t, silent, "--silent")

    CCSH_WRAPPER_ARG0(head_t, v, "-v")
    CCSH_WRAPPER_ARG0(head_t, verbose, "--verbose")

    CCSH_WRAPPER_ARG0(head_t, z, "-z")
    CCSH_WRAPPER_ARG0(head_t, zero_terminated, "--zero-terminated")

    template<typename RATIO>
    CCSH_WRAPPER_ARG1(head_t, c, "-c", quantity<RATIO>, quantity_to_string)
    template<typename RATIO>
    CCSH_WRAPPER_ARG1_FWD(head_t, bytes, c, quantity<RATIO>)

    template<typename RATIO>
    CCSH_WRAPPER_ARG1(head_t, n, "-n", quantity<RATIO>, quantity_to_string)
    template<typename RATIO>
    CCSH_WRAPPER_ARG1_FWD(head_t, lines, n, quantity<RATIO>)

};

using head = command_holder<head_t>;

}
}

#endif // CCSH_CORE_HEAD_HPP_INCLUDED
