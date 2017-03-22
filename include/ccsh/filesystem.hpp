#ifndef CCSH_FILESYSTEM_HPP
#define CCSH_FILESYSTEM_HPP

#ifdef CCSH_FILESYSTEM_BOOST

#ifdef __GNUC__

#pragma push_macro("_GLIBCXX_NOTHROW")
#    undef _GLIBCXX_NOTHROW
#    define _GLIBCXX_NOTHROW
#    include <boost/filesystem.hpp>
#    pragma pop_macro("_GLIBCXX_NOTHROW")
#else
#    include <boost/filesystem.hpp>
#endif

namespace ccsh { namespace fs {
using namespace boost::filesystem;
}

#else

#include <experimental/filesystem>

namespace ccsh { namespace fs {
using namespace std::experimental::filesystem;

}}

#endif


#endif //CCSH_FILESYSTEM_HPP
