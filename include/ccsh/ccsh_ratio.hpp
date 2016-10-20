#ifndef CCSH_RATIO_HPP_INCLUDED
#define CCSH_RATIO_HPP_INCLUDED

#include <string>
#include <ratio>

namespace ccsh
{

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
        return 0;
    }

    constexpr quantity operator-() const
    {
        return quantity(-q);
    }

};


struct blocks : public std::ratio<512, 1>
{
    static constexpr const char * name = "b";
};


struct kilo : public std::kilo
{
    static constexpr const char * name = "kB";
};
struct mega : public std::mega
{
    static constexpr const char * name = "MB";
};
struct giga : public std::giga
{
    static constexpr const char * name = "GB";
};
struct tera : public std::tera
{
    static constexpr const char * name = "TB";
};
struct peta : public std::peta
{
    static constexpr const char * name = "PB";
};
struct exa : public std::exa
{
    static constexpr const char * name = "EB";
};


struct kibi : public std::ratio<1024, 1>
{
    static constexpr const char * name = "k";
};
struct mebi : public std::ratio<1048576, 1>
{
    static constexpr const char * name = "M";
};
struct gibi : public std::ratio<1073741824, 1>
{
    static constexpr const char * name = "G";
};
struct tebi : public std::ratio<1099511627776, 1>
{
    static constexpr const char * name = "T";
};
struct pebi : public std::ratio<1125899906842624, 1>
{
    static constexpr const char * name = "P";
};
struct exbi : public std::ratio<1152921504606846976, 1>
{
    static constexpr const char * name = "E";
};


template<typename RATIO>
std::string quantity_to_string(quantity<RATIO> q)
{
    return std::to_string(q.get()) + RATIO::name;
}


namespace literals
{

constexpr quantity<blocks> operator""_b(unsigned long long q)
{
    return quantity<blocks>(q);
}

constexpr quantity<kilo> operator""_kB(unsigned long long q)
{
    return quantity<kilo>(q);
}
constexpr quantity<mega> operator""_MB(unsigned long long q)
{
    return quantity<mega>(q);
}
constexpr quantity<giga> operator""_GB(unsigned long long q)
{
    return quantity<giga>(q);
}
constexpr quantity<tera> operator""_TB(unsigned long long q)
{
    return quantity<tera>(q);
}
constexpr quantity<peta> operator""_PB(unsigned long long q)
{
    return quantity<peta>(q);
}
constexpr quantity<exa> operator""_EB(unsigned long long q)
{
    return quantity<exa>(q);
}


constexpr quantity<kibi> operator""_k(unsigned long long q)
{
    return quantity<kibi>(q);
}
constexpr quantity<mebi> operator""_M(unsigned long long q)
{
    return quantity<mebi>(q);
}
constexpr quantity<gibi> operator""_G(unsigned long long q)
{
    return quantity<gibi>(q);
}
constexpr quantity<tebi> operator""_T(unsigned long long q)
{
    return quantity<tebi>(q);
}
constexpr quantity<pebi> operator""_P(unsigned long long q)
{
    return quantity<pebi>(q);
}
constexpr quantity<exbi> operator""_E(unsigned long long q)
{
    return quantity<exbi>(q);
}

} // namespace literals

} // namespace ccsh

#endif // CCSH_RATIO_HPP_INCLUDED
