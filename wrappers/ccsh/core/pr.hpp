#ifndef CCSH_CORE_PR_HPP_INCLUDED
#define CCSH_CORE_PR_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {
namespace core {

class pr_t : public wrappers::options_paths<pr_t>
{
    using base = wrappers::options_paths<pr_t>;
    static constexpr const char* name = "pr";

public:

    using base::base;

    // TODO
};

using pr = command_holder<pr_t>;


}
}

#endif // CCSH_CORE_PR_HPP_INCLUDED
