#ifndef CCSH_CORE_CAT_HPP_INCLUDED
#define CCSH_CORE_CAT_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {


class cat_t : public wrappers::options_paths<cat_t>
{
    using base = wrappers::options_paths<cat_t>;
    friend base;
    static constexpr const char * name() { return "cat"; }

public:

    using base::base;

    CCSH_WRAPPER_ARG0(cat_t, A, "-A")
    CCSH_WRAPPER_ARG0(cat_t, show_all, "--show-all")

    CCSH_WRAPPER_ARG0(cat_t, b, "-b")
    CCSH_WRAPPER_ARG0(cat_t, number_nonblank, "--number-nonblank")

    CCSH_WRAPPER_ARG0(cat_t, e, "-e")

    CCSH_WRAPPER_ARG0(cat_t, E, "-E")
    CCSH_WRAPPER_ARG0(cat_t, show_ends, "--show-ends")

    CCSH_WRAPPER_ARG0(cat_t, n, "-n")
    CCSH_WRAPPER_ARG0(cat_t, number, "--number")

    CCSH_WRAPPER_ARG0(cat_t, s, "-s")
    CCSH_WRAPPER_ARG0(cat_t, squeeze_blank, "--squeeze-blank")

    CCSH_WRAPPER_ARG0(cat_t, t, "-t")

    CCSH_WRAPPER_ARG0(cat_t, T, "-T")
    CCSH_WRAPPER_ARG0(cat_t, show_tabs, "--show-tabs")

    CCSH_WRAPPER_ARG0(cat_t, u, "-u")

    CCSH_WRAPPER_ARG0(cat_t, v, "-v")
    CCSH_WRAPPER_ARG0(cat_t, show_nonprinting, "--show-nonprinting")
};

using cat = command_holder<cat_t>;

}  // namespace ccsh

#endif // CCSH_CORE_CAT_HPP_INCLUDED
