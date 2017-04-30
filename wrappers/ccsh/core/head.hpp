#ifndef CCSH_CORE_HEAD_HPP_INCLUDED
#define CCSH_CORE_HEAD_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_ratio.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {


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

    template<typename RATIO> command_holder<head_t>&  c(quantity<RATIO> arg)&  { return add_larg_s("-c", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<head_t>&& c(quantity<RATIO> arg)&& { return add_rarg_s("-c", quantity_to_string(arg)); }

    template<typename RATIO> command_holder<head_t>&  bytes(quantity<RATIO> arg)&  { return add_larg_e("--bytes", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<head_t>&& bytes(quantity<RATIO> arg)&& { return add_rarg_e("--bytes", quantity_to_string(arg)); }

    template<typename RATIO> command_holder<head_t>&  n(quantity<RATIO> arg)&  { return add_larg_s("-n", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<head_t>&& n(quantity<RATIO> arg)&& { return add_rarg_s("-n", quantity_to_string(arg)); }

    template<typename RATIO> command_holder<head_t>&  lines(quantity<RATIO> arg)&  { return add_larg_e("--lines", quantity_to_string(arg)); }
    template<typename RATIO> command_holder<head_t>&& lines(quantity<RATIO> arg)&& { return add_rarg_e("--lines", quantity_to_string(arg)); }
};

using head = command_holder<head_t>;

}  // namespace ccsh

#endif // CCSH_CORE_HEAD_HPP_INCLUDED
