#ifndef CCSH_CORE_OD_HPP_INCLUDED
#define CCSH_CORE_OD_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {
namespace core {


class od_t : public wrappers::options_paths<od_t>
{
    using base = wrappers::options_paths<od_t>;
    friend base;
    static constexpr const char* name = "od";

public:

    using base::base;

    // TODO
};

using od = command_holder<od_t>;


}
}

#endif // CCSH_CORE_OD_HPP_INCLUDED
