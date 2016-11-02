#ifndef CCSH_WRAPPERS_HPP_INCLUDED
#define CCSH_WRAPPERS_HPP_INCLUDED

#include "ccsh_command.hpp"

#define CCSH_WRAPPER_ARG0(comm, method, arg) \
    command_holder<comm>& method() \
    { \
        args.push_back(arg); \
        return static_cast<command_holder<comm>&>(*this); \
    }

#define CCSH_WRAPPER_ARG1(comm, method, argstr, ty, conv) \
    command_holder<comm>& method(ty arg) \
    { \
        args.push_back(argstr); \
        args.push_back(conv(arg)); \
        return static_cast<command_holder<comm>&>(*this); \
    }

#define CCSH_WRAPPER_ARG1_FWD(comm, method, fwd_to, ty) \
    command_holder<comm>& method(ty arg) \
    { \
        return fwd_to(std::move(arg)); \
    }

#define CCSH_WRAPPER_COMMON_CLASS(basecmd, cmdname, cmdstr) \
    namespace hidden { constexpr const char cmdname##_name[] = cmdstr; } \
    using cmdname = basecmd<hidden::cmdname##_name>;

namespace ccsh {
namespace wrappers {

template<typename DERIVED>
class options_paths : public command_native, public command_builder_base
{
protected:
    std::vector<fs::path> paths;

    virtual std::vector<const char*> get_argv() const override final
    {
        std::vector<const char*> argv = command_native::get_argv();
        argv.reserve(argv.size() + paths.size());
        argv.pop_back();
        for(const auto& p : paths)
            argv.push_back(p.c_str());

        argv.push_back(nullptr);
        return argv;
    }

public:

    options_paths()
        : command_native(DERIVED::name)
        , paths()
    { }

    options_paths(std::vector<fs::path> const& paths)
        : command_native(DERIVED::name)
        , paths(paths)
    { }

    options_paths(fs::path const& p)
        : command_native(DERIVED::name)
        , paths({p})
    { }
};

}
}

#endif // CCSH_WRAPPERS_HPP_INCLUDED
