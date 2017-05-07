#ifndef TMP_LIB_CALL_KILL_HPP
#define TMP_LIB_CALL_KILL_HPP

#include <utility>
#include <type_traits>

namespace tmp_lib
{

template<typename FUNC>
class call_kill_t
{
    FUNC func;
public:
    template<typename FUNC2>
    call_kill_t(FUNC2&& func)
        : func(std::forward<FUNC2>(func))
    { }

    template<typename... ARGS2>
    auto operator()(ARGS2&& ... args) -> typename std::remove_reference<decltype(func(std::forward<ARGS2>(args)...))>::type
    {
        auto result = func(std::forward<ARGS2>(args)...);
        func = FUNC{};
        return result;
    }
};

template<typename FUNC>
call_kill_t<FUNC> call_kill(FUNC&& func)
{
    return {std::forward<FUNC>(func)};
}

}

#endif //TMP_LIB_CALL_KILL_HPP
