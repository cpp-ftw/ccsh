#ifndef CCSH_CORE_TAIL_HPP_INCLUDED
#define CCSH_CORE_TAIL_HPP_INCLUDED

#include <chrono>

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_ratio.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {

class tail_t : public wrappers::options_paths<tail_t>
{
    using base = wrappers::options_paths<tail_t>;
    friend base;
    static constexpr const char * name() { return "tail"; }

public:
    enum follow_type
    {
        descriptor,
        name_,
    };

private:
    static constexpr std::array<const char*, 2> follow_type_mapping()
    { return { { "descriptor", "name", } }; };

public:

    using base::base;

    CCSH_WRAPPER_ARG0(tail_t, f, "-f")
    CCSH_WRAPPER_ARG0(tail_t, follow, "--follow")
    CCSH_WRAPPER_ARG1_E(tail_t, follow, "--follow", follow_type, enum_to_string(arg, follow_type_mapping()))

    CCSH_WRAPPER_ARG0(tail_t, F, "-F")

    CCSH_WRAPPER_ARG1_S(tail_t, max_unchanged_stats, "--max-unchanged-stats", unsigned, std::to_string(arg))

    CCSH_WRAPPER_ARG1_S(tail_t, pid, "--pid", int, std::to_string(arg))

    CCSH_WRAPPER_ARG0(tail_t, q, "-q")
    CCSH_WRAPPER_ARG0(tail_t, quiet, "--quiet")
    CCSH_WRAPPER_ARG0(tail_t, silent, "--silent")

    CCSH_WRAPPER_ARG0(tail_t, retry, "--retry")

    CCSH_WRAPPER_ARG1_S(tail_t, s, "-s", std::chrono::seconds, std::to_string(arg.count()))
    CCSH_WRAPPER_ARG1_FWD(tail_t, sleep_interval, s, std::chrono::seconds)

    CCSH_WRAPPER_ARG0(tail_t, v, "-v")
    CCSH_WRAPPER_ARG0(tail_t, verbose, "--verbose")

    CCSH_WRAPPER_ARG0(tail_t, z, "-z")
    CCSH_WRAPPER_ARG0(tail_t, zero_terminated, "--zero-terminated")

    template<typename RATIO> command_holder<tail_t>&  c(quantity<RATIO> arg)&  { return add_larg_s("-c", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<tail_t>&& c(quantity<RATIO> arg)&& { return add_rarg_s("-c", quantity_to_string(arg)); }

    template<typename RATIO> command_holder<tail_t>&  bytes(quantity<RATIO> arg)&  { return add_larg_cat("--bytes", "=", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<tail_t>&& bytes(quantity<RATIO> arg)&& { return add_rarg_cat("--bytes", "=", quantity_to_string(arg)); }

    template<typename RATIO> command_holder<tail_t>&  n(quantity<RATIO> arg)&  { return add_larg_s("-n", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<tail_t>&& n(quantity<RATIO> arg)&& { return add_rarg_s("-n", quantity_to_string(arg)); }

    template<typename RATIO> command_holder<tail_t>&  lines(quantity<RATIO> arg)&  { return add_larg_cat("--lines", "=", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<tail_t>&& lines(quantity<RATIO> arg)&& { return add_rarg_cat("--lines", "=", quantity_to_string(arg)); }
};

using tail = command_holder<tail_t>;

}  // namespace ccsh

#endif // CCSH_CORE_TAIL_HPP_INCLUDED
