#ifndef CCSH_RATIO_HPP_INCLUDED
#define CCSH_RATIO_HPP_INCLUDED

#include <string>
#include <ratio>

namespace ccsh {

template<typename RATIO>
class quantity
{
    int64_t q;
public:
    constexpr explicit quantity(int64_t q)
        : q(q)
    { }

    constexpr int64_t get() const
    {
        return q;
    }

    constexpr quantity operator-() const
    {
        return quantity(-q);
    }
};

#define CCSH_DEFINE_RATIO_PREFIX(prefix, literal) \
struct prefix : public std:: prefix \
{ \
    static constexpr const char * name = #prefix; \
}; \
namespace literals { \
constexpr quantity<prefix> operator"" _##literal(unsigned long long q) \
{ \
    return quantity<prefix>(q); \
} \
}

#define CCSH_DEFINE_RATIO_RATIO(prefix, literal, num) \
struct prefix : public std::ratio<num, 1> \
{ \
    static constexpr const char * name = #prefix; \
}; \
namespace literals { \
constexpr quantity<prefix> operator"" _##literal(unsigned long long q) \
{ \
    return quantity<prefix>(q); \
} \
}

CCSH_DEFINE_RATIO_RATIO(blocks, b, 512)

CCSH_DEFINE_RATIO_PREFIX(kilo, kB)
CCSH_DEFINE_RATIO_PREFIX(mega, MB)
CCSH_DEFINE_RATIO_PREFIX(giga, GB)
CCSH_DEFINE_RATIO_PREFIX(tera, TB)
CCSH_DEFINE_RATIO_PREFIX(peta, PB)
CCSH_DEFINE_RATIO_PREFIX(exa, eB)

CCSH_DEFINE_RATIO_RATIO(kibi, k, 1024)
CCSH_DEFINE_RATIO_RATIO(mebi, M, 1048576)
CCSH_DEFINE_RATIO_RATIO(gibi, G, 1073741824)
CCSH_DEFINE_RATIO_RATIO(tebi, T, 1099511627776)
CCSH_DEFINE_RATIO_RATIO(pebi, P, 1125899906842624)
CCSH_DEFINE_RATIO_RATIO(exbi, E, 1152921504606846976)

#undef CCSH_DEFINE_RATIO_PREFIX
#undef CCSH_DEFINE_RATIO_RATIO

template<typename RATIO>
std::string quantity_to_string(quantity<RATIO> q)
{
    return std::to_string(q.get()) + RATIO::name;
}
} // namespace ccsh

#endif // CCSH_RATIO_HPP_INCLUDED
