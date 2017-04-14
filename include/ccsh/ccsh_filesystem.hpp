#ifndef CCSH_FILESYSTEM_HPP_INCLUDED
#define CCSH_FILESYSTEM_HPP_INCLUDED

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

namespace ccsh { namespace fs {
using namespace boost::filesystem;
using boost::system::error_code;
}}

#else

#include <system_error>
#include <experimental/filesystem>

namespace ccsh { namespace fs {

using namespace std::experimental::filesystem;
using std::error_code;

path relative(path const& p, path const& base, fs::error_code& ec);
}}

#endif

namespace ccsh { namespace fs {

path self_lexically_relative(path const& self, path const& base);
path self_lexically_normal(path const& self);
}}

#endif // CCSH_FILESYSTEM_HPP_INCLUDED
