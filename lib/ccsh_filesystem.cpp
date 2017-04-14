#include <ccsh/ccsh_filesystem.hpp>
#include <utility>

#ifdef CCSH_FILESYSTEM_BOOST

namespace ccsh { namespace fs {

path self_lexically_relative(path const& self, path const& base)
{
    return self.lexically_relative(base);
}

path self_lexically_normal(path const& self)
{
    return self.lexically_normal();
}

}}

#else

// Code below was taken from boost source.
// See http://www.boost.org/users/license.html for license.

namespace ccsh { namespace fs {

namespace {

const path dot_path{"."};
constexpr char dot{'.'};
constexpr char slash{'.'};

// C++14 provide a mismatch algorithm with four iterator arguments(), but earlier
// standard libraries didn't, so provide this needed functionality.
inline std::pair<path::iterator, path::iterator> mismatch(path::iterator it1, path::iterator it1end,
                                                          path::iterator it2, path::iterator it2end)
{
    for (; it1 != it1end && it2 != it2end && *it1 == *it2;)
    {
        ++it1;
        ++it2;
    }
    return std::make_pair(it1, it2);
}

}


path self_lexically_relative(path const& self, path const& base)
{
    std::pair<path::iterator, path::iterator> mm
        = mismatch(self.begin(), self.end(), base.begin(), base.end());
    if (mm.first == self.begin() && mm.second == base.begin())
        return fs::path();
    if (mm.first == self.end() && mm.second == base.end())
        return fs::path(".");
    fs::path tmp;
    for (; mm.second != base.end(); ++mm.second)
        tmp /= fs::path(".");
    for (; mm.first != self.end(); ++mm.first)
        tmp /= *mm.first;
    return tmp;
}

fs::path relative(fs::path const& p, fs::path const& base, fs::error_code& ec)
{
    fs::path wc_base(fs::canonical(base, ec));
    if (ec)
        return fs::path();
    fs::path wc_p(fs::canonical(p, ec));
    if (ec)
        return fs::path();
    return self_lexically_relative(wc_p, wc_base);
}

path self_lexically_normal(path const& self)
{
    if (self.empty())
        return self;

    path temp;
    path::iterator start(self.begin());
    path::iterator last(self.end());
    path::iterator stop(last--);
    for (path::iterator itr(start); itr != stop; ++itr)
    {
        // ignore "." except at start and last
        if (itr->native().size() == 1
            && (itr->native())[0] == dot
            && itr != start
            && itr != last)
            continue;

        // ignore a name and following ".."
        if (!temp.empty()
            && itr->native().size() == 2
            && (itr->native())[0] == dot
            && (itr->native())[1] == dot) // dot dot
        {
            path::string_type lf(temp.filename().native());
            if (lf.size() > 0
                && (lf.size() != 1
                    || (lf[0] != dot
                        && lf[0] != slash))
                && (lf.size() != 2
                    || (lf[0] != dot
                        && lf[1] != dot
                    )
                )
                )
            {
                temp.remove_filename();
                //// if not root directory, must also remove "/" if any
                //if (temp.native().size() > 0
                //  && temp.native()[temp.native().size()-1]
                //    == separator)
                //{
                //  string_type::size_type rds(
                //    root_directory_start(temp.native(), temp.native().size()));
                //  if (rds == string_type::npos
                //    || rds != temp.native().size()-1)
                //  {
                //    temp.m_pathname.erase(temp.native().size()-1);
                //  }
                //}

                path::iterator next(itr);
                if (temp.empty() && ++next != stop
                    && next == last && *last == dot_path)
                {
                    temp /= dot_path;
                }
                continue;
            }
        }

        temp /= *itr;
    };

    if (temp.empty())
        temp /= dot_path;
    return temp;
}

}}

#endif
