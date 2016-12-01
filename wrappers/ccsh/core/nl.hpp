#ifndef CCSH_CORE_NL_HPP_INCLUDED
#define CCSH_CORE_NL_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>
#include <ccsh/ccsh_regex.hpp>

namespace ccsh {
namespace core {

class nl_t : public wrappers::options_paths<nl_t>
{
    using base = wrappers::options_paths<nl_t>;
    static constexpr const char* name = "nl";
    friend base;

public:

    enum numbering_style
    {
        a,  all_lines,
        t,  non_empty_lines,
        n_, no_lines,
    };

    enum numbering_format
    {
        ln, left_justified,
        rn, right_justified,
        rz, right_justified_zeros,
    };

private:

    static constexpr std::array<const char*, 6> numbering_style_mapping()
        { return { "a", "a", "t", "t", "n", "n" }; };
    static constexpr std::array<const char*, 6> numbering_format_mapping()
        { return { "ln", "ln", "rn", "rn", "rz", "rz" }; };

    static const char* flag_to_str(numbering_style flag)
    {
        return enum_to_string(flag, numbering_style_mapping());
    }

    static const char* flag_to_str(numbering_format flag)
    {
        return enum_to_string(flag, numbering_format_mapping());
    }

public:

    using base::base;

    CCSH_WRAPPER_ARG1(nl_t, b, "-b", numbering_style, flag_to_str)
    CCSH_WRAPPER_ARG1_FWD(nl_t, body_numbering, b, numbering_style)

    CCSH_WRAPPER_ARG1(nl_t, b, "-b", regex<regex_type::basic> const&, 'p' + std::string)
    CCSH_WRAPPER_ARG1_FWD(nl_t, body_numbering, b, regex<regex_type::basic>)

    CCSH_WRAPPER_ARG1(nl_t, h, "-h", numbering_style, flag_to_str)
    CCSH_WRAPPER_ARG1_FWD(nl_t, header_numbering, h, numbering_style)

    CCSH_WRAPPER_ARG1(nl_t, h, "-h", regex<regex_type::basic> const&, 'p' + std::string)
    CCSH_WRAPPER_ARG1_FWD(nl_t, header_numbering, h, regex<regex_type::basic>)

    CCSH_WRAPPER_ARG1(nl_t, f, "-f", numbering_style, flag_to_str)
    CCSH_WRAPPER_ARG1_FWD(nl_t, footer_numbering, f, numbering_style)

    CCSH_WRAPPER_ARG1(nl_t, f, "-f", regex<regex_type::basic> const&, 'p' + std::string)
    CCSH_WRAPPER_ARG1_FWD(nl_t, footer_numbering, f, regex<regex_type::basic>)

    CCSH_WRAPPER_ARG1(nl_t, n, "-n", numbering_format, flag_to_str)
    CCSH_WRAPPER_ARG1_FWD(nl_t, number_format, n, numbering_format)

    CCSH_WRAPPER_ARG1(nl_t, i, "-i", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(nl_t, line_increment, i, unsigned)

    CCSH_WRAPPER_ARG1(nl_t, l, "-l", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(nl_t, join_blank_lines, l, unsigned)

    CCSH_WRAPPER_ARG1(nl_t, v, "-v", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(nl_t, starting_line_number, v, unsigned)

    CCSH_WRAPPER_ARG1(nl_t, w, "-w", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(nl_t, number_width, w, unsigned)

    CCSH_WRAPPER_ARG1(nl_t, d, "-d", std::string, std::move)
    CCSH_WRAPPER_ARG1_FWD(nl_t, section_delimiter, d, std::string)

    CCSH_WRAPPER_ARG1(nl_t, s, "-s", std::string, std::move)
    CCSH_WRAPPER_ARG1_FWD(nl_t, number_separator, s, std::string)

    CCSH_WRAPPER_ARG0(nl_t, p, "-p")
    CCSH_WRAPPER_ARG0(nl_t, no_renumber, "--no-renumber")
};

using nl = command_holder<nl_t>;

}
}

#endif // CCSH_CORE_NL_HPP_INCLUDED
