#ifndef CCSH_CORE_CKSUM_HPP_INCLUDED
#define CCSH_CORE_CKSUM_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {


class cksum_t : public wrappers::options_paths<cksum_t>
{
    using base = wrappers::options_paths<cksum_t>;
    friend base;
    static constexpr const char * name() { return "cksum"; }

public:

    using base::base;

};

using cksum = command_holder<cksum_t>;

}

#endif // CCSH_CORE_CKSUM_HPP_INCLUDED
