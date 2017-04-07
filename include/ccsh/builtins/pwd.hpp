#ifndef CCSH_PWD_HPP_INCLUDED
#define CCSH_PWD_HPP_INCLUDED

#include "../ccsh_command.hpp"

namespace ccsh {

class pwd_t : public internal::command_builtin
{
    fs::path p;
    bool verbatim_pwd = true;
    bool helpflag = false;

public:
    pwd_t(fs::path const& p = get_home()) : p(p)
    {}

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

    int runx(int in, int out, int err) const override final;
};

using pwd = command_holder<pwd_t>;


}

#endif // CCSH_PWD_HPP_INCLUDED
