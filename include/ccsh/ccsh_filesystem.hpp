#ifndef CCSH_CCSH_FILESYSTEM_HPP
#define CCSH_CCSH_FILESYSTEM_HPP

#ifdef CCSH_FILESYSTEM_BOOST

#ifdef __GNUC__
#    pragma push_macro("_GLIBCXX_NOTHROW")
#    undef _GLIBCXX_NOTHROW
#    define _GLIBCXX_NOTHROW
#    include <boost/filesystem.hpp>
#    pragma pop_macro("_GLIBCXX_NOTHROW")
#else
#    include <boost/filesystem.hpp>
#endif

namespace ccsh {
namespace fs {
using namespace boost::filesystem;
using boost::system::error_code;
using boost::system::system_category;
} // namespace fs
} // namespace ccsh

#else

#if __cplusplus < 201703L
#include <experimental/filesystem>
#include <system_error>

namespace ccsh {
namespace fs {
using namespace std::experimental::filesystem;
using std::error_code;
using std::system_category;

path weakly_canonical(const path& p);
path weakly_canonical(const path& p, std::error_code& ec);

} // namespace fs
} // namespace ccsh



#else
#include <filesystem>

namespace ccsh {
namespace fs {
using namespace std::filesystem;
using std::error_code;
} // namespace fs
} // namespace ccsh

#endif

#endif

namespace ccsh {
namespace fs {

path self_lexically_relative(path const& self, path const& base);
path self_lexically_normal(path const& self);

#if !defined(CCSH_FILESYSTEM_BOOST) || BOOST_VERSION < 106000
path relative(path const& p, path const& base, fs::error_code& ec);
path safe_relative(path const& p, path const& base);
#endif

struct path_equivalent
{
    bool operator()(const ccsh::fs::path &lhs, const ccsh::fs::path &rhs) const
    {
        return ccsh::fs::equivalent(lhs, rhs);
    }
};

struct path_hash
{
    size_t operator()(const ccsh::fs::path &p) const
    {
        return ccsh::fs::hash_value(p);
    }
};

} // namespace fs
} // namespace ccsh

#endif // CCSH_CCSH_FILESYSTEM_HPP
