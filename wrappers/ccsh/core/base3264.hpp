#ifndef CCSH_CORE_BASE3264_HPP_INCLUDED
#define CCSH_CORE_BASE3264_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {
namespace core {

template<const char* NAME>
class base_t : public wrappers::options_paths<base_t<NAME>>
{
    using base = wrappers::options_paths<base_t<NAME>>;
    friend base;
    static constexpr const char * name() { return NAME; }

public:

    using base::base;

    // sorry
#define args base::args

    CCSH_WRAPPER_ARG1(base_t, w, "-w", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(base_t, wrap, w, unsigned)

    CCSH_WRAPPER_ARG0(base_t, d, "-d")
    CCSH_WRAPPER_ARG0(base_t, decode, "--decode")

    CCSH_WRAPPER_ARG0(base_t, i, "-i")
    CCSH_WRAPPER_ARG0(base_t, ignore_garbage, "--ignore_garbage")

#undef args

};

CCSH_WRAPPER_COMMON_CLASS(base_t, base32_t, "base32")
CCSH_WRAPPER_COMMON_CLASS(base_t, base64_t, "base64")

using base32 = command_holder<base32_t>;
using base64 = command_holder<base64_t>;

}
}

#endif // CCSH_CORE_BASE3264_HPP_INCLUDED
