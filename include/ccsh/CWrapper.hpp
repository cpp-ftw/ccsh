#ifndef CCSH_CWRAPPER_HPP
#define CCSH_CWRAPPER_HPP

/** https://github.com/dobragab/CWrapper */

#include <exception>
#include <utility>

#define HAS_STATIC_MEMBER_DETECTOR(member)                                  \
template<typename T>                                                        \
class HAS_STATIC_MEMBER_DETECTOR_CLASS_ ## member                           \
{                                                                           \
    struct two_ints_type { int two_ints_1, two_ints_2; };                   \
    template<typename U>                                                    \
    static two_ints_type has_func_helper(...) { }                           \
    template<typename U>                                                    \
    static int has_func_helper(int, decltype(U::member)* = nullptr)         \
    { return 0; }                                                           \
public:                                                                     \
    static constexpr bool value =                                           \
        sizeof(decltype(has_func_helper<T>(0))) == sizeof(int);             \
}

#define HAS_STATIC_MEMBER(type, member)                                     \
    (HAS_STATIC_MEMBER_DETECTOR_CLASS_ ## member<type>::value)

#define HAS_NESTED_TYPE_DETECTOR(member)                                    \
template<typename T>                                                        \
class HAS_NESTED_TYPE_DETECTOR_CLASS_ ## member                             \
{                                                                           \
    struct two_ints_type { int two_ints_1, two_ints_2; };                   \
    template<typename U>                                                    \
    static two_ints_type has_func_helper(...) { }                           \
    template<typename U>                                                    \
    static int has_func_helper(int, typename U::member* = nullptr)          \
    { return 0; }                                                           \
public:                                                                     \
    static constexpr bool value =                                           \
        sizeof(decltype(has_func_helper<T>(0))) == sizeof(int);             \
}

#define HAS_NESTED_TYPE(type, member)                                       \
    (HAS_NESTED_TYPE_DETECTOR_CLASS_ ## member<type>::value)

namespace CW {

enum class CWrapperType
{
    Implicit,
    Explicit,
    Get,
};

struct CWrapperException : public std::exception
{
    const char* what() const noexcept override
    {
        return "CWrapperException";
    }
};

template<
    typename H,
    typename F,
    CWrapperType TY = CWrapperType::Get,
    bool C = true>
class CWrapperFriend
{

    template<
        typename HANDLE_T,
        typename BASE,
        bool CONSTSAFE>
    struct ArrowHandler
    {
    };

    template<
        typename PTR_T,
        typename BASE>
    struct ArrowHandler<PTR_T*, BASE, false>
    {
        PTR_T* operator->() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<
        typename PTR_T,
        typename BASE>
    struct ArrowHandler<PTR_T*, BASE, true>
    {
        PTR_T* operator->()
        { return static_cast<BASE*>(this)->ptr; }
        PTR_T const* operator->() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<
        typename HANDLE_T,
        typename BASE,
        CWrapperType TYPE,
        bool CONSTSAFE>
    struct ConversionHandler : public ArrowHandler<HANDLE_T, BASE, CONSTSAFE>
    {
    };

    template<
        typename HANDLE_T,
        typename BASE,
        bool CONSTSAFE>
    struct ConversionHandler<HANDLE_T, BASE, CWrapperType::Implicit, CONSTSAFE>
        : public ArrowHandler<HANDLE_T, BASE, CONSTSAFE>
    {
        operator HANDLE_T() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<
        typename HANDLE_T,
        typename BASE,
        bool CONSTSAFE>
    struct ConversionHandler<HANDLE_T, BASE, CWrapperType::Explicit, CONSTSAFE>
        : public ArrowHandler<HANDLE_T, BASE, CONSTSAFE>
    {
        explicit operator HANDLE_T() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<
        typename HANDLE_T,
        typename BASE,
        bool CONSTSAFE>
    struct ConversionHandler<HANDLE_T, BASE, CWrapperType::Get, CONSTSAFE>
        : public ArrowHandler<HANDLE_T, BASE, CONSTSAFE>
    {
        HANDLE_T get() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<
        typename PTR_T,
        typename BASE>
    struct ConversionHandler<PTR_T*, BASE, CWrapperType::Implicit, true>
        : public ArrowHandler<PTR_T*, BASE, true>
    {
        operator PTR_T*()
        { return static_cast<BASE*>(this)->ptr; }
        operator PTR_T const*() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<
        typename PTR_T,
        typename BASE>
    struct ConversionHandler<PTR_T*, BASE, CWrapperType::Explicit, true>
        : public ArrowHandler<PTR_T*, BASE, true>
    {
        explicit operator PTR_T*()
        { return static_cast<BASE*>(this)->ptr; }
        explicit operator PTR_T const*() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<
        typename PTR_T,
        typename BASE>
    struct ConversionHandler<PTR_T*, BASE, CWrapperType::Get, true>
        : public ArrowHandler<PTR_T*, BASE, true>
    {
        PTR_T* get()
        { return static_cast<BASE*>(this)->ptr; }
        PTR_T const* get() const
        { return static_cast<const BASE*>(this)->ptr; }
    };

    template<bool b, typename T>
    struct hider
    {
        static constexpr bool value = b;
    };

// Works exactly like std::enable if. I didn't want to include
// type_traits for this one and COND
    template<bool cond, typename T = void>
    struct enabler
    {
    };
    template<typename T>
    struct enabler<true, T>
    {
        using type = T;
    };

    template<bool condition, typename TRUE_TYPE, typename FALSE_TYPE>
    struct COND
    {
        using type = TRUE_TYPE;
    };
    template<typename TRUE_TYPE, typename FALSE_TYPE>
    struct COND<false, TRUE_TYPE, FALSE_TYPE>
    {
        using type = FALSE_TYPE;
    };

    template<
        typename HANDLE_T,
        typename FUNCTIONS,
        CWrapperType TYPE,
        bool CONSTSAFE,
        typename EXCEPTION_T,
        typename INVALID_T,
        typename VALIDATE_T,
        typename COPY_T,
        bool HAS_COPY>
    class CWrapperBase
        : public ConversionHandler<
            HANDLE_T,
            CWrapperBase<HANDLE_T, FUNCTIONS, TYPE, CONSTSAFE, EXCEPTION_T,
                INVALID_T, VALIDATE_T, COPY_T, HAS_COPY>,
            TYPE,
            CONSTSAFE>
    {
        friend ConversionHandler<
            HANDLE_T,
            CWrapperBase<HANDLE_T, FUNCTIONS, TYPE, CONSTSAFE, EXCEPTION_T,
                INVALID_T, VALIDATE_T, COPY_T, HAS_COPY>,
            TYPE,
            CONSTSAFE>;
    protected:
        HANDLE_T ptr;

    public:

        CWrapperBase()
            :
            ptr{INVALID_T::invalid_value}
        { }

        explicit CWrapperBase(HANDLE_T ptr)
            :
            ptr{ptr}
        {
            if (!VALIDATE_T::validate_func(ptr))
                throw typename EXCEPTION_T::exception{};
        }

        template<typename DUMMY = void,
            typename = typename enabler<hider<HAS_COPY, DUMMY>::value>::type>
        CWrapperBase(CWrapperBase const& other)
            :
            CWrapperBase{COPY_T::copy_func(other.ptr)}
        { }

        // This one is needed to prevent variadic ctor to be
        // called when you want to copy a non-const object.
        template<typename DUMMY = void,
            typename = typename enabler<hider<HAS_COPY, DUMMY>::value>::type>
        CWrapperBase(CWrapperBase& other)
            :
            CWrapperBase{COPY_T::copy_func(other.ptr)}
        { }


        template<typename DUMMY = void,
            typename = typename enabler<hider<!HAS_COPY, DUMMY>::value>::type>
        CWrapperBase(CWrapperBase const& other, void** ptr = nullptr) = delete;

        // This one is needed to prevent variadic ctor to be
        // called when you want to copy a non-const object.
        template<typename DUMMY = void,
            typename = typename enabler<hider<!HAS_COPY, DUMMY>::value>::type>
        CWrapperBase(CWrapperBase& other, void** ptr = nullptr) = delete;

        template<typename... ARGS>
        explicit CWrapperBase(ARGS&& ... args)
            :
            CWrapperBase{FUNCTIONS::ctor_func(std::forward<ARGS>(args)...)}
        { }

        CWrapperBase(CWrapperBase&& old)
            :
            CWrapperBase{old.ptr}
        {
            old.ptr = INVALID_T::invalid_value;
        }

        HANDLE_T release() noexcept
        {
            HANDLE_T result = ptr;
            ptr = INVALID_T::invalid_value;
            return result;
        }

        ~CWrapperBase()
        {
            if (VALIDATE_T::validate_func(ptr))
                FUNCTIONS::dtor_func(ptr);
        }

        CWrapperBase& operator=(CWrapperBase other)
        {
            std::swap(ptr, other.ptr);
            return *this;
        }
    };

    HAS_NESTED_TYPE_DETECTOR(exception);

    HAS_STATIC_MEMBER_DETECTOR(copy_func);

    HAS_STATIC_MEMBER_DETECTOR(invalid_value);

    HAS_STATIC_MEMBER_DETECTOR(validate_func);

    struct default_exception_type
    {
        using exception = CWrapperException;
    };

    template<typename T>
    struct default_invalid_value
    {
        static constexpr T invalid_value = 0;
    };
    template<typename T>
    struct default_invalid_value<T*>
    {
        static constexpr T* invalid_value = nullptr;
    };

    template<typename T, typename INVALID_T>
    struct default_validate_func
    {
        static constexpr bool validate_func(T ptr)
        {
            return ptr != INVALID_T::invalid_value;
        }
    };

// this one is never called, but necessary to compile if copy ctor is deleted
    template<typename T>
    struct default_copy_func
    {
        static constexpr T copy_func(T const& other)
        {
            return other;
        }
    };

    using E = typename COND<HAS_NESTED_TYPE(F, exception),
        F, default_exception_type>::type;
    using D = typename COND<HAS_STATIC_MEMBER(F, invalid_value),
        F, default_invalid_value<H>>::type;
    using V = typename COND<HAS_STATIC_MEMBER(F, validate_func),
        F, default_validate_func<H, D>>::type;

    using COPY = typename COND<HAS_STATIC_MEMBER(F, copy_func),
        F, default_copy_func<H>>::type;

public:

    using type = CWrapperBase<H, F, TY, C, E, D, V, COPY, HAS_STATIC_MEMBER(F, copy_func)>;
};

template<
    typename HANDLE_T,
    typename FUNCTIONS,
    CWrapperType TYPE = CWrapperType::Get,
    bool CONSTSAFE = true>
using CWrapper = typename CWrapperFriend<HANDLE_T, FUNCTIONS, TYPE, CONSTSAFE>::type;

} // namespace CW

#undef HAS_STATIC_MEMBER_DETECTOR
#undef HAS_STATIC_MEMBER
#undef HAS_NESTED_TYPE_DETECTOR
#undef HAS_NESTED_TYPE


#endif // CCSH_CWRAPPER_HPP
