#ifndef CCSH_BUILTINS_PWD_HPP
#define CCSH_BUILTINS_PWD_HPP

#include "../ccsh_command.hpp"

#include <utility>

namespace ccsh {

class pwd_t : public internal::command_builtin
{
    fs::path p;
    bool verbatim_pwd = true;
    bool helpflag = false;

public:
    explicit pwd_t(fs::path p = get_home())
        : p(std::move(p))
    { }

    command_holder<pwd_t>& P()
    {
        verbatim_pwd = false;
        return static_cast<command_holder<pwd_t>&>(*this);
    }

    command_holder<pwd_t>& L()
    {
        verbatim_pwd = true;
        return static_cast<command_holder<pwd_t>&>(*this);
    }

    command_holder<pwd_t>& help()
    {
        helpflag = true;
        return static_cast<command_holder<pwd_t>&>(*this);
    }

    int runx(int in, int out, int err) const final;
};

using pwd = command_holder<pwd_t>;


} // namespace ccsh

#endif // CCSH_BUILTINS_PWD_HPP
