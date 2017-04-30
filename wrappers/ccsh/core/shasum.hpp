#ifndef CCSH_CORE_SHASUM_HPP_INCLUDED
#define CCSH_CORE_SHASUM_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {


template<const char* NAME>
class shasum_t : public wrappers::options_paths<shasum_t<NAME>>
{
    using base = wrappers::options_paths<shasum_t<NAME>>;
    friend base;
    static constexpr const char * name() { return NAME; }

public:

    using base::base;

    CCSH_WRAPPER_ARG0(shasum_t, b, "-b")
    CCSH_WRAPPER_ARG0(shasum_t, binary, "--binary")

    CCSH_WRAPPER_ARG0(shasum_t, c, "-c")
    CCSH_WRAPPER_ARG0(shasum_t, check, "--check")

    CCSH_WRAPPER_ARG0(shasum_t, ignore_missing, "--ignore-missing")

    CCSH_WRAPPER_ARG0(shasum_t, quiet, "--quiet")

    CCSH_WRAPPER_ARG0(shasum_t, status, "--status")

    CCSH_WRAPPER_ARG0(shasum_t, tag, "--tag")

    CCSH_WRAPPER_ARG0(shasum_t, t, "-t")
    CCSH_WRAPPER_ARG0(shasum_t, text, "--text")

    CCSH_WRAPPER_ARG0(shasum_t, w, "-w")
    CCSH_WRAPPER_ARG0(shasum_t, warn, "--warn")

    CCSH_WRAPPER_ARG0(shasum_t, strict, "--strict")

};

CCSH_WRAPPER_COMMON_CLASS(shasum_t, md5sum_t,    "md5sum")
CCSH_WRAPPER_COMMON_CLASS(shasum_t, sha1sum_t,   "sha1sum")
CCSH_WRAPPER_COMMON_CLASS(shasum_t, sha224sum_t, "sha224sum")
CCSH_WRAPPER_COMMON_CLASS(shasum_t, sha256sum_t, "sha256sum")
CCSH_WRAPPER_COMMON_CLASS(shasum_t, sha384sum_t, "sha384sum")
CCSH_WRAPPER_COMMON_CLASS(shasum_t, sha512sum_t, "sha512sum")

using md5sum = command_holder<md5sum_t>;
using sha1sum = command_holder<sha1sum_t>;
using sha224sum = command_holder<sha224sum_t>;
using sha256sum = command_holder<sha256sum_t>;
using sha384sum = command_holder<sha384sum_t>;
using sha512sum = command_holder<sha512sum_t>;

}  // namespace ccsh

#endif // CCSH_CORE_SHASUM_HPP_INCLUDED
