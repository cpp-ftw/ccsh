#ifndef CCSH_CORE_TAIL_HPP_INCLUDED
#define CCSH_CORE_TAIL_HPP_INCLUDED

#include <chrono>

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>
#include <ccsh/ccsh_ratio.hpp>

namespace ccsh {
namespace core {


class tail_t : public wrappers::options_paths<tail_t>
{
    using base = wrappers::options_paths<tail_t>;
    friend base;
    static constexpr const char* name = "tail";

public:
    enum follow_type
    {
        descriptor,
        name_,
    };

private:
    static constexpr const char * follow_type_mapping[] =
    {
        "descriptor",
        "name",
    };

public:

    using base::base;

    CCSH_WRAPPER_ARG0(tail_t, f, "-f")
    CCSH_WRAPPER_ARG0(tail_t, follow, "--follow")
    command_holder<tail_t>& follow(follow_type type)
    {
        args.push_back(std::string("--follow=") + enum_to_string(type, follow_type_mapping));
        return static_cast<command_holder<tail_t>&>(*this);
    }

    CCSH_WRAPPER_ARG0(tail_t, F, "-F")

    CCSH_WRAPPER_ARG1(tail_t, max_unchanged_stats, "--max-unchanged-stats", unsigned, std::to_string)

    CCSH_WRAPPER_ARG1(tail_t, pid, "--pid", int, std::to_string)

    CCSH_WRAPPER_ARG0(tail_t, q, "-q")
    CCSH_WRAPPER_ARG0(tail_t, quiet, "--quiet")
    CCSH_WRAPPER_ARG0(tail_t, silent, "--silent")

    CCSH_WRAPPER_ARG0(tail_t, retry, "--retry")

    command_holder<tail_t>& s(std::chrono::seconds arg)
    {
        args.push_back("-s");
        args.push_back(std::to_string(arg.count()));
        return static_cast<command_holder<tail_t>&>(*this);
    }
    CCSH_WRAPPER_ARG1_FWD(tail_t, sleep_interval, s, std::chrono::seconds)

    CCSH_WRAPPER_ARG0(tail_t, v, "-v")
    CCSH_WRAPPER_ARG0(tail_t, verbose, "--verbose")

    CCSH_WRAPPER_ARG0(tail_t, z, "-z")
    CCSH_WRAPPER_ARG0(tail_t, zero_terminated, "--zero-terminated")

    template<typename RATIO>
    CCSH_WRAPPER_ARG1(tail_t, c, "-c", quantity<RATIO>, quantity_to_string)
    template<typename RATIO>
    CCSH_WRAPPER_ARG1_FWD(tail_t, bytes, c, quantity<RATIO>)

    template<typename RATIO>
    CCSH_WRAPPER_ARG1(tail_t, n, "-n", quantity<RATIO>, quantity_to_string)
    template<typename RATIO>
    CCSH_WRAPPER_ARG1_FWD(tail_t, lines, n, quantity<RATIO>)

};

using tail = command_holder<tail_t>;

}
}

#endif // CCSH_CORE_TAIL_HPP_INCLUDED
