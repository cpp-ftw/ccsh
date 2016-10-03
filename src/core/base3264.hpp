#ifndef CCSH_CORE_BASE3264_HPP_INCLUDED
#define CCSH_CORE_BASE3264_HPP_INCLUDED

#include "../ccsh_command.hpp"
#include "../ccsh_wrappers.hpp"

namespace ccsh {
namespace core {


class base32_t : public wrappers::options_paths<base32_t>
{
    using base = wrappers::options_paths<base32_t>;
    friend base;
    static constexpr const char* name = "base32";

public:

    using base::base;

    CCSH_WRAPPER_ARG1(base32_t, w, "-w", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(base32_t, wrap, w, unsigned)

    CCSH_WRAPPER_ARG0(base32_t, d, "-d")
    CCSH_WRAPPER_ARG0(base32_t, decode, "--decode")

    CCSH_WRAPPER_ARG0(base32_t, i, "-i")
    CCSH_WRAPPER_ARG0(base32_t, ignore_garbage, "--ignore_garbage")

};

using base32 = command_holder<base32_t>;

class base64_t : public wrappers::options_paths<base64_t>
{
    using base = wrappers::options_paths<base64_t>;
    friend base;
    static constexpr const char* name = "base64";

public:

    using base::base;

    CCSH_WRAPPER_ARG1(base64_t, w, "-w", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(base64_t, wrap, w, unsigned)

    CCSH_WRAPPER_ARG0(base64_t, d, "-d")
    CCSH_WRAPPER_ARG0(base64_t, decode, "--decode")

    CCSH_WRAPPER_ARG0(base64_t, i, "-i")
    CCSH_WRAPPER_ARG0(base64_t, ignore_garbage, "--ignore_garbage")

};

using base64 = command_holder<base64_t>;


}
}

#endif // CCSH_CORE_BASE3264_HPP_INCLUDED
