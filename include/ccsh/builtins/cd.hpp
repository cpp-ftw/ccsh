#ifndef CCSH_CD_HPP_INCLUDED
#define CCSH_CD_HPP_INCLUDED

#include "../ccsh_command.hpp"

namespace ccsh {

class cd_t : public internal::command_base, public internal::command_builder_base
{
    fs::path p;
    bool eflag = true;
    bool follow_symlinks = true;
    bool helpflag = false;

public:
    cd_t(fs::path const& p = get_home()) : p(p)
    {}

    command_holder<cd_t>& P()
    {
        follow_symlinks = false;
        return static_cast<command_holder<cd_t>&>(*this);
    }

    command_holder<cd_t>& L()
    {
        follow_symlinks = true;
        return static_cast<command_holder<cd_t>&>(*this);
    }

    command_holder<cd_t>& e()
    {
        eflag = true;
        return static_cast<command_holder<cd_t>&>(*this);
    }

    command_holder<cd_t>& help()
    {
        helpflag = true;
        return static_cast<command_holder<cd_t>&>(*this);
    }

    int runx(int in, int out, int err) const override final;
};

using cd = command_holder<cd_t>;


}

#endif // CCSH_CD_HPP_INCLUDED
