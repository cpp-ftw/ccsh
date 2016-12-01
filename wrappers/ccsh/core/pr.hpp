#ifndef CCSH_CORE_PR_HPP_INCLUDED
#define CCSH_CORE_PR_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {
namespace core {

class pr_t : public wrappers::options_paths<pr_t>
{
    using base = wrappers::options_paths<pr_t>;
    static constexpr const char* name = "pr";
    friend base;

public:

    using base::base;

    command_holder<pr_t>& pages(int first)
    {
        args.push_back("--pages=" + std::to_string(first));
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& pages(int first, int last)
    {
        args.push_back("--pages=" + std::to_string(first) + ":" + std::to_string(last));
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& columns(int col)
    {
        args.push_back("--columns=" + std::to_string(col));
        return static_cast<command_holder<pr_t>&>(*this);
    }

    CCSH_WRAPPER_ARG0(pr_t, a, "-a")
    CCSH_WRAPPER_ARG0(pr_t, across, "--across")

    CCSH_WRAPPER_ARG0(pr_t, c, "-c")
    CCSH_WRAPPER_ARG0(pr_t, show_control_chars, "--show-control-chars")

    CCSH_WRAPPER_ARG0(pr_t, d, "-d")
    CCSH_WRAPPER_ARG0(pr_t, double_space, "--double-space")

    CCSH_WRAPPER_ARG1(pr_t, D, "-D", std::string, std::move)
    CCSH_WRAPPER_ARG1_FWD(pr_t, date_format, D, std::string)

    CCSH_WRAPPER_ARG0(pr_t, e, "-e")
    CCSH_WRAPPER_ARG0(pr_t, expand_tabs, "--expand-tabs")

    command_holder<pr_t>& e(char tabchar)
    {
        args.push_back(std::string("-e") + tabchar);
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& e(char tabchar, int tabwidth)
    {
        args.push_back(std::string("-e") + tabchar + std::to_string(tabwidth));
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& expand_tabs(char tabchar)
    {
        return this->e(tabchar);
    }

    command_holder<pr_t>& expand_tabs(char tabchar, int tabwidth)
    {
        return this->e(tabchar, tabwidth);
    }

    CCSH_WRAPPER_ARG0(pr_t, f, "-f")
    CCSH_WRAPPER_ARG0(pr_t, F, "-F")
    CCSH_WRAPPER_ARG0(pr_t, form_feed, "--form-feed")

    CCSH_WRAPPER_ARG1(pr_t, h, "-h", std::string, std::move)
    CCSH_WRAPPER_ARG1_FWD(pr_t, header, h, std::string)

    CCSH_WRAPPER_ARG0(pr_t, i, "-i")
    CCSH_WRAPPER_ARG0(pr_t, output_tabs, "--output-tabs")

    command_holder<pr_t>& i(char tabchar)
    {
        args.push_back(std::string("-i") + tabchar);
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& i(char tabchar, int tabwidth)
    {
        args.push_back(std::string("-i") + tabchar + std::to_string(tabwidth));
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& output_tabs(char tabchar)
    {
        return this->i(tabchar);
    }

    command_holder<pr_t>& output_tabs(char tabchar, int tabwidth)
    {
        return this->i(tabchar, tabwidth);
    }

    CCSH_WRAPPER_ARG0(pr_t, J, "-J")
    CCSH_WRAPPER_ARG0(pr_t, join_lines, "--join-lines")

    CCSH_WRAPPER_ARG1(pr_t, l, "-l", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(pr_t, length, l, unsigned)

    CCSH_WRAPPER_ARG0(pr_t, m, "-m")
    CCSH_WRAPPER_ARG0(pr_t, merge, "--merge")

    CCSH_WRAPPER_ARG0(pr_t, n, "-n")
    CCSH_WRAPPER_ARG0(pr_t, number_lines, "--number-lines")

    command_holder<pr_t>& n(char tabchar)
    {
        args.push_back(std::string("-n") + tabchar);
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& n(char tabchar, int tabwidth)
    {
        args.push_back(std::string("-n") + tabchar + std::to_string(tabwidth));
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& number_lines(char tabchar)
    {
        return this->n(tabchar);
    }

    command_holder<pr_t>& number_lines(char tabchar, int tabwidth)
    {
        return this->n(tabchar, tabwidth);
    }

    CCSH_WRAPPER_ARG1(pr_t, N, "-N", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(pr_t, first_line_number, N, unsigned)

    CCSH_WRAPPER_ARG1(pr_t, o, "-o", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(pr_t, indent, o, unsigned)

    CCSH_WRAPPER_ARG0(pr_t, r, "-r")
    CCSH_WRAPPER_ARG0(pr_t, no_file_warnings, "--no-file-warnings")

    CCSH_WRAPPER_ARG0(pr_t, s, "-s")
    CCSH_WRAPPER_ARG0(pr_t, separator, "--separator")

    command_holder<pr_t>& s(char tabchar)
    {
        args.push_back(std::string("-s") + tabchar);
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& separator(char tabchar)
    {
        return this->s(tabchar);
    }

    CCSH_WRAPPER_ARG0(pr_t, S, "-S")
    CCSH_WRAPPER_ARG0(pr_t, sep_string, "--sep-string")

    command_holder<pr_t>& S(std::string const& str)
    {
        args.push_back("-S" + std::move(str));
        return static_cast<command_holder<pr_t>&>(*this);
    }

    command_holder<pr_t>& sep_string(std::string const& str)
    {
        return this->S(str);
    }

    CCSH_WRAPPER_ARG0(pr_t, t, "-t")
    CCSH_WRAPPER_ARG0(pr_t, omit_header, "--omit-header")

    CCSH_WRAPPER_ARG0(pr_t, T, "-T")
    CCSH_WRAPPER_ARG0(pr_t, omit_pagination, "--omit-pagination")

    CCSH_WRAPPER_ARG0(pr_t, v, "-v")
    CCSH_WRAPPER_ARG0(pr_t, show_nonprinting, "--show-nonprinting")

    CCSH_WRAPPER_ARG1(pr_t, w, "-w", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(pr_t, width, w, unsigned)

    CCSH_WRAPPER_ARG1(pr_t, W, "-W", unsigned, std::to_string)
    CCSH_WRAPPER_ARG1_FWD(pr_t, page_width, W, unsigned)

};

using pr = command_holder<pr_t>;


}
}

#endif // CCSH_CORE_PR_HPP_INCLUDED
