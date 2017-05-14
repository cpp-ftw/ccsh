#ifndef CCSH_CCSH_WRAPPERS_HPP
#define CCSH_CCSH_WRAPPERS_HPP

#include "ccsh_command.hpp"
#include <string>
#include <utility>

#define MAGIC(x) (void)swallow{0, ((void)(x), 0)...}

#define CCSH_WRAPPER_ARG0(comm, method, arg) \
    command_holder <comm>&  method() &  { return this->add_larg(arg); } \
    command_holder <comm>&& method() && { return this->add_rarg(arg); }


#define CCSH_WRAPPER_ARG1_S(comm, method, argstr, ty, userarg) \
    command_holder <comm>&  method(ty arg) &  { return this->add_larg_s(argstr, (userarg)); } \
    command_holder <comm>&& method(ty arg) && { return this->add_rarg_s(argstr, (userarg)); }

#define CCSH_WRAPPER_ARG1_E(comm, method, argstr, ty, userarg) \
    command_holder <comm>&  method(ty arg) &  { return this->add_larg_cat(argstr, "=", (userarg)); } \
    command_holder <comm>&& method(ty arg) && { return this->add_rarg_cat(argstr, "=", (userarg)); }

#define CCSH_WRAPPER_ARG1_N(comm, method, argstr, ty, userarg) \
    command_holder <comm>&  method(ty arg) &  { return this->add_larg_cat(argstr, (userarg)); } \
    command_holder <comm>&& method(ty arg) && { return this->add_rarg_cat(argstr, (userarg)); }

#define CCSH_WRAPPER_ARG1_FWD(comm, method, fwd_to, ty) \
    command_holder<comm>&  method(ty arg)&  { return static_cast<command_holder<comm>&>(this->fwd_to(std::move(arg))); } \
    command_holder<comm>&& method(ty arg)&& { return std::move(this->fwd_to(std::move(arg))); }


#define CCSH_WRAPPER_ARG2_EC(comm, method, argstr, ty1, userarg1, ty2, userarg2) \
    command_holder <comm>&  method(ty1 arg1, ty2 arg2) &  { return this->add_larg_cat(argstr, "=", (userarg1), ":", (userarg2)); } \
    command_holder <comm>&& method(ty1 arg1, ty2 arg2) && { return this->add_rarg_cat(argstr, "=", (userarg1), ":", (userarg2)); }

#define CCSH_WRAPPER_ARG2_EN(comm, method, argstr, ty1, userarg1, ty2, userarg2) \
    command_holder <comm>&  method(ty1 arg1, ty2 arg2) &  { return this->add_larg_cat(argstr, "=", (userarg1), (userarg2)); } \
    command_holder <comm>&& method(ty1 arg1, ty2 arg2) && { return this->add_rarg_cat(argstr, "=", (userarg1), (userarg2)); }

#define CCSH_WRAPPER_ARG2_NN(comm, method, argstr, ty1, userarg1, ty2, userarg2) \
    command_holder <comm>&  method(ty1 arg1, ty2 arg2) &  { return this->add_larg_cat(argstr, (userarg1), (userarg2)); } \
    command_holder <comm>&& method(ty1 arg1, ty2 arg2) && { return this->add_rarg_cat(argstr, (userarg1), (userarg2)); }

#define CCSH_WRAPPER_ARG2_NC(comm, method, argstr, ty1, userarg1, ty2, userarg2) \
    command_holder <comm>&  method(ty1 arg1, ty2 arg2) &  { return this->add_larg_cat(argstr, (userarg1), ":", (userarg2)); } \
    command_holder <comm>&& method(ty1 arg1, ty2 arg2) && { return this->add_rarg_cat(argstr, (userarg1), ":", (userarg2)); }

#define CCSH_WRAPPER_ARG2_NE(comm, method, argstr, ty1, userarg1, ty2, userarg2) \
    command_holder <comm>&  method(ty1 arg1, ty2 arg2) &  { return this->add_larg_cat(argstr, (userarg1), "=", (userarg2)); } \
    command_holder <comm>&& method(ty1 arg1, ty2 arg2) && { return this->add_rarg_cat(argstr, (userarg1), "=", (userarg2)); }


#define CCSH_WRAPPER_COMMON_CLASS(basecmd, cmdname, cmdstr) \
    class cmdname : public basecmd<cmdname> { static constexpr const char * name() { return cmdstr; } };


namespace ccsh {
namespace wrappers {

template<typename DERIVED>
class options_paths : public internal::command_native
{
protected:
    std::vector<fs::path> paths;

    std::vector<const tchar_t*> get_argv() const final
    {
        std::vector<const tchar_t*> argv = internal::command_native::get_argv();
        argv.reserve(argv.size() + paths.size());
        argv.pop_back();
        for (const auto& p : paths)
            argv.emplace_back(p.c_str());

        argv.emplace_back(nullptr);
        return argv;
    }

    // -x or --xx
    command_holder <DERIVED>& add_larg(const char* arg)
    {
        add_arg(arg);
        return static_cast<command_holder<DERIVED>&>(*this);
    }

    command_holder <DERIVED>&& add_rarg(const char* arg)
    {
        add_arg(arg);
        return std::move(static_cast<command_holder<DERIVED>&>(*this));
    }

    // -x arg or --xx arg
    template<typename ARG>
    command_holder <DERIVED>& add_larg_s(const char* name, ARG&& arg)
    {
        add_arg(name);
        add_arg(std::forward<ARG>(arg));
        return static_cast<command_holder<DERIVED>&>(*this);
    }

    template<typename ARG>
    command_holder <DERIVED>&& add_rarg_s(const char* name, ARG&& arg)
    {
        add_arg(name);
        add_arg(std::forward<ARG>(arg));
        return std::move(static_cast<command_holder<DERIVED>&>(*this));
    }

    template<typename... ARG>
    command_holder <DERIVED>& add_larg_cat(const char* name, ARG&& ... arg)
    {
        std::string res = name;
        using swallow = int[];
        MAGIC(res += arg);
        add_arg(std::move(res));
        return static_cast<command_holder<DERIVED>&>(*this);
    }
    template<typename... ARG>
    command_holder <DERIVED>&& add_rarg_cat(const char* name, ARG&& ... arg)
    {
        std::string res = name;
        using swallow = int[];
        MAGIC(res += arg);
        add_arg(std::move(res));
        return std::move(static_cast<command_holder<DERIVED>&>(*this));
    }

public:

    options_paths()
        : command_native(DERIVED::name())
    { }

    explicit options_paths(std::vector<fs::path> const& paths)
        : command_native(DERIVED::name())
        , paths(fs::expand(paths))
    { }

    explicit options_paths(fs::path const& p)
        : command_native(DERIVED::name())
        , paths(fs::expand(p))
    { }

    command_holder <DERIVED>& version()&
    { return add_larg("--version"); }
    command_holder <DERIVED>&& version()&&
    { return add_rarg("--version"); }

    command_holder <DERIVED>& help()&
    { return add_larg("--help"); }
    command_holder <DERIVED>&& help()&&
    { return add_rarg("--help"); }

};

#undef MAGIC

} // namespace wrappers
} // namespace ccsh

#endif // CCSH_CCSH_WRAPPERS_HPP
