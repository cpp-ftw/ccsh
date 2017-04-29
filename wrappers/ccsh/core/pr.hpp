#ifndef CCSH_CORE_PR_HPP_INCLUDED
#define CCSH_CORE_PR_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {

class pr_t : public wrappers::options_paths<pr_t>
{
    using base = wrappers::options_paths<pr_t>;
    static constexpr const char * name() { return "pr"; }
    friend base;

public:

    using base::base;

    CCSH_WRAPPER_ARG1_E(pr_t, pages, "--pages", int, std::to_string(arg))
    CCSH_WRAPPER_ARG2_EC(pr_t, pages, "--pages", int, std::to_string(arg1), int, std::to_string(arg2))

    CCSH_WRAPPER_ARG1_E(pr_t, columns, "--columns", int, std::to_string(arg))

    CCSH_WRAPPER_ARG0(pr_t, a, "-a")
    CCSH_WRAPPER_ARG0(pr_t, across, "--across")

    CCSH_WRAPPER_ARG0(pr_t, c, "-c")
    CCSH_WRAPPER_ARG0(pr_t, show_control_chars, "--show-control-chars")

    CCSH_WRAPPER_ARG0(pr_t, d, "-d")
    CCSH_WRAPPER_ARG0(pr_t, double_space, "--double-space")

    CCSH_WRAPPER_ARG1_S(pr_t, D, "-D", std::string, std::move(arg))
    CCSH_WRAPPER_ARG1_FWD(pr_t, date_format, D, std::string)

    CCSH_WRAPPER_ARG0(pr_t, e, "-e")
    CCSH_WRAPPER_ARG0(pr_t, expand_tabs, "--expand-tabs")

    CCSH_WRAPPER_ARG1_N(pr_t, e, "-e", char, arg)
    CCSH_WRAPPER_ARG2_NN(pr_t, e, "-e", char, arg1, int, std::to_string(arg2))

    CCSH_WRAPPER_ARG1_E(pr_t, expand_tabs, "--expand-tabs", char, arg)
    CCSH_WRAPPER_ARG2_EN(pr_t, expand_tabs, "--expand-tabs", char, arg1, int, std::to_string(arg2))

    CCSH_WRAPPER_ARG0(pr_t, f, "-f")
    CCSH_WRAPPER_ARG0(pr_t, F, "-F")
    CCSH_WRAPPER_ARG0(pr_t, form_feed, "--form-feed")

    CCSH_WRAPPER_ARG1_S(pr_t, h, "-h", std::string, std::move(arg))
    CCSH_WRAPPER_ARG1_FWD(pr_t, header, h, std::string)

    CCSH_WRAPPER_ARG0(pr_t, i, "-i")
    CCSH_WRAPPER_ARG0(pr_t, output_tabs, "--output-tabs")

    CCSH_WRAPPER_ARG1_N(pr_t, i, "-i", char, arg)
    CCSH_WRAPPER_ARG2_NN(pr_t, i, "-i", char, arg1, int, std::to_string(arg2))

    CCSH_WRAPPER_ARG1_E(pr_t, output_tabs, "--output-tabs", char, arg)
    CCSH_WRAPPER_ARG2_EN(pr_t, output_tabs, "--output-tabs", char, arg1, int, std::to_string(arg2))

    CCSH_WRAPPER_ARG0(pr_t, J, "-J")
    CCSH_WRAPPER_ARG0(pr_t, join_lines, "--join-lines")

    CCSH_WRAPPER_ARG1_S(pr_t, l, "-l", unsigned, std::to_string(arg))
    CCSH_WRAPPER_ARG1_FWD(pr_t, length, l, unsigned)

    CCSH_WRAPPER_ARG0(pr_t, m, "-m")
    CCSH_WRAPPER_ARG0(pr_t, merge, "--merge")

    CCSH_WRAPPER_ARG0(pr_t, n, "-n")
    CCSH_WRAPPER_ARG0(pr_t, number_lines, "--number-lines")

    CCSH_WRAPPER_ARG1_N(pr_t, n, "-n", char, arg)
    CCSH_WRAPPER_ARG2_NN(pr_t, n, "-n", char, arg1, int, std::to_string(arg2))

    CCSH_WRAPPER_ARG1_E(pr_t, number_lines, "--number-lines", char, arg)
    CCSH_WRAPPER_ARG2_EN(pr_t, number_lines, "--number-lines", char, arg1, int, std::to_string(arg2))

    CCSH_WRAPPER_ARG1_S(pr_t, N, "-N", unsigned, std::to_string(arg))
    CCSH_WRAPPER_ARG1_FWD(pr_t, first_line_number, N, unsigned)

    CCSH_WRAPPER_ARG1_S(pr_t, o, "-o", unsigned, std::to_string(arg))
    CCSH_WRAPPER_ARG1_FWD(pr_t, indent, o, unsigned)

    CCSH_WRAPPER_ARG0(pr_t, r, "-r")
    CCSH_WRAPPER_ARG0(pr_t, no_file_warnings, "--no-file-warnings")

    CCSH_WRAPPER_ARG0(pr_t, s, "-s")
    CCSH_WRAPPER_ARG1_N(pr_t, s, "-s", char, arg)

    CCSH_WRAPPER_ARG0(pr_t, separator, "--separator")
    CCSH_WRAPPER_ARG1_E(pr_t, separator, "--separator", char, arg)

    CCSH_WRAPPER_ARG0(pr_t, S, "-S")
    CCSH_WRAPPER_ARG1_N(pr_t, S, "-S", std::string const&, arg)

    CCSH_WRAPPER_ARG0(pr_t, sep_string, "--sep-string")
    CCSH_WRAPPER_ARG1_E(pr_t, sep_string, "--sep-string", std::string const&, arg)

    CCSH_WRAPPER_ARG0(pr_t, t, "-t")
    CCSH_WRAPPER_ARG0(pr_t, omit_header, "--omit-header")

    CCSH_WRAPPER_ARG0(pr_t, T, "-T")
    CCSH_WRAPPER_ARG0(pr_t, omit_pagination, "--omit-pagination")

    CCSH_WRAPPER_ARG0(pr_t, v, "-v")
    CCSH_WRAPPER_ARG0(pr_t, show_nonprinting, "--show-nonprinting")

    CCSH_WRAPPER_ARG1_S(pr_t, w, "-w", unsigned, std::to_string(arg))
    CCSH_WRAPPER_ARG1_FWD(pr_t, width, w, unsigned)

    CCSH_WRAPPER_ARG1_S(pr_t, W, "-W", unsigned, std::to_string(arg))
    CCSH_WRAPPER_ARG1_FWD(pr_t, page_width, W, unsigned)

};

using pr = command_holder<pr_t>;


}

#endif // CCSH_CORE_PR_HPP_INCLUDED
