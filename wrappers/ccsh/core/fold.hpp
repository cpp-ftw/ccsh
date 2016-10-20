#ifndef CCSH_CORE_FOLD_HPP_INCLUDED
#define CCSH_CORE_FOLD_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {
namespace core {


class fold_t : public wrappers::options_paths<fold_t>
{
    using base = wrappers::options_paths<fold_t>;
    friend base;
    static constexpr const char* name = "fold";

public:

    using base::base;

    CCSH_WRAPPER_ARG0(fold_t, b, "-b")
    CCSH_WRAPPER_ARG0(fold_t, bytes, "--bytes")

    CCSH_WRAPPER_ARG0(fold_t, s, "-s")
    CCSH_WRAPPER_ARG0(fold_t, spaces, "--spaces")

    CCSH_WRAPPER_ARG1(fold_t, w, "-w", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(fold_t, width, w, unsigned)

};

using fold = command_holder<fold_t>;

}
}

#endif // CCSH_CORE_FOLD_HPP_INCLUDED
