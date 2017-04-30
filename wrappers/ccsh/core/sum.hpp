#ifndef CCSH_CORE_SUM_HPP_INCLUDED
#define CCSH_CORE_SUM_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {


class sum_t : public wrappers::options_paths<sum_t>
{
    using base = wrappers::options_paths<sum_t>;
    friend base;
    static constexpr const char * name() { return "sum"; }

public:

    using base::base;

    CCSH_WRAPPER_ARG0(sum_t, r, "-r")

    CCSH_WRAPPER_ARG0(sum_t, s, "-s")
    CCSH_WRAPPER_ARG0(sum_t, sysv, "--sysv")

};

using sum = command_holder<sum_t>;

}  // namespace ccsh

#endif // CCSH_CORE_SUM_HPP_INCLUDED
