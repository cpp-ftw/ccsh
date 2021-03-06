#include <ccsh/ccsh_filesystem.hpp>
#include <utility>

#if defined(CCSH_FILESYSTEM_BOOST) && BOOST_VERSION >= 106000

namespace ccsh {
namespace fs {

path self_lexically_relative(path const& self, path const& base)
{
    return self.lexically_relative(base);
}

path self_lexically_normal(path const& self)
{
    return self.lexically_normal();
}

} // namespace fs
} // namespace ccsh

#else

// Code below was taken from boost source.
// See http://www.boost.org/users/license.html for license.

namespace ccsh {
namespace fs {

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


bool error(int error_num, const path& p, error_code* ec, const char* message)
{
    if (!error_num)
    {
        if (ec != nullptr) ec->clear();
    }
    else
    { //  error
        if (ec == nullptr)
            throw filesystem_error(message, p, error_code(error_num, system_category()));
        else
            ec->assign(error_num, system_category());
    }
    return error_num != 0;
}

path weakly_canonical_impl(const path& p, error_code* ec)
{
#ifdef CCSH_FILESYSTEM_BOOST
    static constexpr auto file_type_none = file_type::status_error;
    static constexpr auto file_type_not_found = file_type::file_not_found;
#else
    static constexpr auto file_type_none = file_type::none;
    static constexpr auto file_type_not_found = file_type::not_found;
#endif
    path head(p);
    path tail;
    error_code tmp_ec;
    path::iterator itr = p.end();

    for (; !head.empty(); --itr)
    {
        file_status head_status = status(head, tmp_ec);
        if (error(head_status.type() == file_type_none, head, ec, "boost::filesystem::weakly_canonical"))
            return path();
        if (head_status.type() != file_type_not_found)
            break;
        head.remove_filename();
    }

    bool tail_has_dots = false;
    for (; itr != p.end(); ++itr)
    {
        tail /= *itr;
        // for a later optimization, track if any dot or dot-dot elements are present
        if (itr->native().size() <= 2
            && itr->native()[0] == dot
            && (itr->native().size() == 1 || itr->native()[1] == dot))
            tail_has_dots = true;
    }

    if (head.empty())
        return self_lexically_normal(p);
    head = canonical(head, tmp_ec);
    if (error(tmp_ec.value(), head, ec, "boost::filesystem::weakly_canonical"))
        return path();
    return tail.empty()
           ? head
           : (tail_has_dots  // optimization: only normalize if tail had dot or dot-dot element
              ? self_lexically_normal(head/tail)
              : head/tail);
}


} // namespace


#if __cplusplus < 201703L

path weakly_canonical(const path& p)
{
    return weakly_canonical_impl(p, nullptr);
}
path weakly_canonical(const path& p, error_code& ec)
{
    return weakly_canonical_impl(p, &ec);
}

#endif


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
    fs::path wc_base(fs::weakly_canonical(base, ec));
    if (ec)
        return fs::path();
    fs::path wc_p(fs::weakly_canonical(p, ec));
    if (ec)
        return fs::path();
    return self_lexically_relative(wc_p, wc_base);
}

fs::path safe_relative(fs::path const& p, fs::path const& base)
{
    fs::error_code ec;
    fs::path result = relative(p, base, ec);
    if (ec)
        return p;
    return result;
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
            if (!lf.empty()
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

} // namespace fs
} // namespace ccsh

#endif
