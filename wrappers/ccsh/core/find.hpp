#ifndef CCSH_FIND_HPP_INCLUDED
#define CCSH_FIND_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {

class find_t : public wrappers::options_paths<find_t>
{
    using base = wrappers::options_paths<find_t>;
    friend base;
    static constexpr const char* name() { return "find"; }

public:
    using base::base;
};

using find = command_holder<find_t>;

}

#endif // CCSH_FIND_HPP_INCLUDED
