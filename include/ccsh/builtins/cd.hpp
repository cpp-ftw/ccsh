#ifndef CCSH_BUILTINS_CD_HPP
#define CCSH_BUILTINS_CD_HPP

#include "../ccsh_command.hpp"

#include <utility>

namespace ccsh {

class cd_t : public internal::command_builtin
{
    fs::path p;
    bool eflag = true;
    bool follow_symlinks = true;
    bool helpflag = false;

public:
    explicit cd_t(fs::path p = get_home())
        : p(std::move(p))
    { }

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

    int runx(int in, int out, int err) const final;
};

using cd = command_holder<cd_t>;


} // namespace ccsh

#endif // CCSH_BUILTINS_CD_HPP
