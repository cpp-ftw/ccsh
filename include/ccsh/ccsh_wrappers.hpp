#ifndef CCSH_WRAPPERS_HPP_INCLUDED
#define CCSH_WRAPPERS_HPP_INCLUDED

#include "ccsh_command.hpp"
#include <utility>
#include <string>

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
    command_holder<comm>&  method(ty arg)&  { return this->fwd_to(std::move(arg)); } \
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


#define CCSH_WRAPPER_COMMON_CLASS(basecmd, cmdname, cmdstr) \
    namespace hidden { constexpr const char cmdname##_name[] = cmdstr; } \
    using cmdname = basecmd<hidden::cmdname##_name>;

namespace ccsh {
namespace wrappers {

template<typename DERIVED>
class options_paths : public internal::command_native
{
protected:
    std::vector<fs::path> paths;

    virtual std::vector<const char*> get_argv() const override final
    {
        std::vector<const char*> argv = internal::command_native::get_argv();
        argv.reserve(argv.size() + paths.size());
        argv.pop_back();
        for (const auto& p : paths)
            argv.push_back(p.c_str());

        argv.push_back(nullptr);
        return argv;
    }

    template<typename ENUM, std::size_t N>
    static const char* enum_to_string(ENUM val, const char* const (& mapping)[N])
    {
        if (val < 0 || val >= N)
            return "";
        return mapping[val];
    }

    template<typename ENUM, std::size_t N>
    static const char* enum_to_string(ENUM val, std::array<const char*, N> const& mapping)
    {
        if (val < 0 || val >= N)
            return "";
        return mapping[val];
    }

    // -x or --xx
    command_holder <DERIVED>& add_larg(const char* arg)
    {
        args.emplace_back(arg);
        return static_cast<command_holder<DERIVED>&>(*this);
    }

    command_holder <DERIVED>&& add_rarg(const char* arg)
    {
        args.emplace_back(arg);
        return std::move(static_cast<command_holder<DERIVED>&>(*this));
    }

    // -x arg or --xx arg
    template<typename ARG>
    command_holder <DERIVED>& add_larg_s(const char* name, ARG&& arg)
    {
        args.emplace_back(name);
        args.push_back(arg);
        return static_cast<command_holder<DERIVED>&>(*this);
    }

    template<typename ARG>
    command_holder <DERIVED>&& add_rarg_s(const char* name, ARG&& arg)
    {
        args.emplace_back(name);
        args.push_back(arg);
        return std::move(static_cast<command_holder<DERIVED>&>(*this));
    }

    template<typename... ARG>
    command_holder <DERIVED>& add_larg_cat(const char* name, ARG&& ... arg)
    {
        std::string res = name;
        using swallow = int[];
        MAGIC(res += arg);
        args.push_back(res);
        return static_cast<command_holder<DERIVED>&>(*this);
    }
    template<typename... ARG>
    command_holder <DERIVED>&& add_rarg_cat(const char* name, ARG&& ... arg)
    {
        std::string res = name;
        using swallow = int[];
        MAGIC(res += arg);
        args.push_back(res);
        return std::move(static_cast<command_holder<DERIVED>&>(*this));
    }

public:

    options_paths()
        : command_native(DERIVED::name())
        , paths()
    { }

    options_paths(std::vector<fs::path> const& paths)
        : command_native(DERIVED::name())
        , paths(fs::expand(paths))
    { }

    options_paths(fs::path const& p)
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

}
}

#endif // CCSH_WRAPPERS_HPP_INCLUDED
