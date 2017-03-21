#ifndef CCSH_FILESYSTEM_HPP
#define CCSH_FILESYSTEM_HPP

#ifdef __GNUC__

#pragma push_macro("_GLIBCXX_NOTHROW")
#    undef _GLIBCXX_NOTHROW
#    define _GLIBCXX_NOTHROW
#    include <boost/filesystem.hpp>
#    pragma pop_macro("_GLIBCXX_NOTHROW")
#else
#    include <boost/filesystem.hpp>
#endif

#endif //CCSH_FILESYSTEM_HPP
