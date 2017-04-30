#ifndef CCSH_CORE_LS_HPP_INCLUDED
#define CCSH_CORE_LS_HPP_INCLUDED

#include <ccsh/ccsh_command.hpp>
#include <ccsh/ccsh_wrappers.hpp>

namespace ccsh {

namespace hidden {

// must be template to keep header-only structure
// this way the static variables can be defined in a header

template<typename DUMMY = void>
class ls_t : public wrappers::options_paths<ls_t<DUMMY>>
{
    using base = wrappers::options_paths<ls_t<DUMMY>>;
    friend base;
    static constexpr const char * name() { return "ls"; }

public:
    enum format_styles
    {
        long_,
        verbose,
        single_column,
        vertical,
        commas,
        across,
        horizontal
    };

private:
    static constexpr const char* format_styles_mapping[] =
    {
        "long",
        "verbose",
        "single-column",
        "vertical",
        "commas",
        "across",
        "horizontal"
    };

public:
    enum time_styles
    {
        format_,
        full_iso,
        long_iso,
        iso,
        posix_style,
        // locale = 5
    };

private:
    static constexpr const char* time_styles_mapping[] =
    {
        "format",
        "full-iso",
        "long-iso",
        "iso",
        "posix-style",
        "locale"
    };

public:
    enum time_type
    {
        ctime,
        status,
        atime,
        access,
        use
    };

private:
    static constexpr const char* time_type_mapping[] =
    {
        "ctime",
        "status",
        "atime",
        "access",
        "use"
    };

public:
    enum sort_type
    {
        size_,
        time_,
        version_,
        extension,
        // none = 4
    };

private:
    static constexpr const char* sort_type_mapping[] =
    {
        "size",
        "time",
        "version",
        "extension",
        "none"
    };

public:
    enum quoting_styles
    {
        literal_,
        shell,
        shell_always,
        shell_escape,
        shell_escape_always,
        c_,
        escape_,
        clocale,
        // locale = 8
    };

private:
    static constexpr const char* quoting_styles_mapping[] =
    {
        "literal",
        "shell",
        "shell-always",
        "shell-escape",
        "shell-escape-always",
        "c",
        "escape",
        "clocale",
        "locale"
    };

public:
    enum indicator_styles
    {
        slash,
        file_type_,
        classify_,
        // none = 3
    };

private:
    static constexpr const char* indicator_styles_mapping[] =
    {
        "slash",
        "file_type",
        "classify",
        "none"
    };

public:
    enum color_type
    {
        auto_,
        always,
        // none = 2
    };

private:
    static constexpr const char* color_type_mapping[] =
    {
        "auto",
        "always",
        "none"
    };

    struct none_t
    {
        constexpr operator color_type() const { return color_type(2); }
        constexpr operator indicator_styles() const { return indicator_styles(3); }
        constexpr operator sort_type() const { return sort_type(4); }
    };

    struct locale_t
    {
        constexpr operator time_styles() const { return time_styles(5); }
        constexpr operator quoting_styles() const { return quoting_styles(8); }
    };

public:

    static constexpr none_t none{};
    static constexpr locale_t locale{};

public:

    using base::base;

    // 1. Which fields are listed

    CCSH_WRAPPER_ARG0(ls_t, a, "-a")
    CCSH_WRAPPER_ARG0(ls_t, all, "--all")

    CCSH_WRAPPER_ARG0(ls_t, A, "-A")
    CCSH_WRAPPER_ARG0(ls_t, almost_all, "--almost-all")

    CCSH_WRAPPER_ARG0(ls_t, B, "-B")
    CCSH_WRAPPER_ARG0(ls_t, ignore_backups, "--ignore-backups")

    CCSH_WRAPPER_ARG0(ls_t, d, "-d")
    CCSH_WRAPPER_ARG0(ls_t, directory, "--directory")

    CCSH_WRAPPER_ARG0(ls_t, H, "-H")
    CCSH_WRAPPER_ARG0(ls_t, dereference_command_line, "--dereference-command-line")

    CCSH_WRAPPER_ARG0(ls_t, dereference_command_line_symlink_to_dir, "--dereference-command-line-symlink-to-dir")
    CCSH_WRAPPER_ARG0(ls_t, group_directories_first, "--group-directories-first")

    CCSH_WRAPPER_ARG1_E(ls_t, hide, "--hide", std::string const&, arg)

    CCSH_WRAPPER_ARG1_S(ls_t, I, "-I", std::string, std::move(arg))
    CCSH_WRAPPER_ARG1_FWD(ls_t, ignore, I, std::string)

    CCSH_WRAPPER_ARG0(ls_t, L, "-L")
    CCSH_WRAPPER_ARG0(ls_t, dereference, "--dereference")

    // 2. What information is listed

    CCSH_WRAPPER_ARG0(ls_t, author, "--author")

    CCSH_WRAPPER_ARG0(ls_t, D, "-D")
    CCSH_WRAPPER_ARG0(ls_t, dired, "--dired")

    CCSH_WRAPPER_ARG0(ls_t, full_time, "--full-time")

    CCSH_WRAPPER_ARG0(ls_t, g, "-g")

    CCSH_WRAPPER_ARG0(ls_t, G, "-G")
    CCSH_WRAPPER_ARG0(ls_t, no_group, "--no-group")

    CCSH_WRAPPER_ARG0(ls_t, h, "-h")
    CCSH_WRAPPER_ARG0(ls_t, human_readable, "--human-readable")

    CCSH_WRAPPER_ARG0(ls_t, i, "-i")
    CCSH_WRAPPER_ARG0(ls_t, inode, "--inode")

    CCSH_WRAPPER_ARG0(ls_t, l, "-l") // --format=long; --format=verbose

    CCSH_WRAPPER_ARG0(ls_t, n, "-n")
    CCSH_WRAPPER_ARG0(ls_t, numeric_uid_gid, "--numeric-uid-gid")

    CCSH_WRAPPER_ARG0(ls_t, o, "-o")

    CCSH_WRAPPER_ARG0(ls_t, s, "-s")
    CCSH_WRAPPER_ARG0(ls_t, size, "--size")

    CCSH_WRAPPER_ARG0(ls_t, si, "-si")

    CCSH_WRAPPER_ARG0(ls_t, Z, "-Z")
    CCSH_WRAPPER_ARG0(ls_t, context, "--context")

    // 3. Sorting the output

    CCSH_WRAPPER_ARG1_E(ls_t, sort, "--sort", sort_type, enum_to_string(arg, sort_type_mapping))

    CCSH_WRAPPER_ARG0(ls_t, c, "-c") // --time=ctime; --time=status
    CCSH_WRAPPER_ARG0(ls_t, f, "-f")

    CCSH_WRAPPER_ARG0(ls_t, r, "-r")
    CCSH_WRAPPER_ARG0(ls_t, reverse, "--reverse")

    CCSH_WRAPPER_ARG1_E(ls_t, time, "--time", time_type, enum_to_string(arg, time_type_mapping))

    CCSH_WRAPPER_ARG0(ls_t, S, "-S") // --sort=size
    CCSH_WRAPPER_ARG0(ls_t, t, "-t") // --sort=time
    CCSH_WRAPPER_ARG0(ls_t, u, "-u") // --time=atime; --time=access; --time=use
    CCSH_WRAPPER_ARG0(ls_t, U, "-U") // --sort=none
    CCSH_WRAPPER_ARG0(ls_t, v, "-v") // --sort=version
    CCSH_WRAPPER_ARG0(ls_t, X, "-X") // --sort=extension

    // 5. General output formatting

    CCSH_WRAPPER_ARG1_E(ls_t, format, "--format", format_styles, base::enum_to_string(arg, format_styles_mapping))

    CCSH_WRAPPER_ARG0(ls_t, one, "-1") // --format=single-column
    CCSH_WRAPPER_ARG0(ls_t, C, "-C")   // --format=vertical

    CCSH_WRAPPER_ARG0(ls_t, color, "--color")   // --color=always
    CCSH_WRAPPER_ARG1_E(ls_t, color, "--color", color_type, base::enum_to_string(arg, color_type_mapping))

    CCSH_WRAPPER_ARG0(ls_t, F, "-F")   // --indicator-style=classify
    CCSH_WRAPPER_ARG0(ls_t, classify, "--classify")

    CCSH_WRAPPER_ARG0(ls_t, file_type, "--file-type")

    CCSH_WRAPPER_ARG1_E(ls_t, indicator_style, "--indicator-style", indicator_styles, base::enum_to_string(arg, indicator_styles_mapping))

    // TODO: --block-size="'1kB" (apostrophe for separating commas)
    template<typename RATIO> command_holder <ls_t>& block_size(quantity<RATIO> arg)& { return this->add_larg_cat("--block-size", "=", (quantity_to_string(arg))); }
    template<typename RATIO> command_holder <ls_t>&& block_size(quantity<RATIO> arg)&& { return this->add_rarg_cat("--block-size", "=", (quantity_to_string(arg))); }

    CCSH_WRAPPER_ARG0(ls_t, k, "-k")
    CCSH_WRAPPER_ARG0(ls_t, kibibytes, "--kibibytes")

    CCSH_WRAPPER_ARG0(ls_t, m, "-m") // --format=commas
    CCSH_WRAPPER_ARG0(ls_t, p, "-p") // --indicator-style=slash
    CCSH_WRAPPER_ARG0(ls_t, x, "-x") // --format=across; --format=horizontal

    CCSH_WRAPPER_ARG1_S(ls_t, T, "-T", unsigned, std::to_string(arg))
    CCSH_WRAPPER_ARG1_FWD(ls_t, tabsize, T, unsigned)

    CCSH_WRAPPER_ARG1_S(ls_t, w, "-wrappers", unsigned, std::to_string(arg))
    CCSH_WRAPPER_ARG1_FWD(ls_t, width, w, unsigned)

    // 6. Formatting file timestamps

    CCSH_WRAPPER_ARG1_E(ls_t, time_style, "--time-style", time_styles, base::enum_to_string(arg, time_styles_mapping))

    // 7. Formatting the file names

    CCSH_WRAPPER_ARG0(ls_t, b, "-b")   // --quoting-style=escape
    CCSH_WRAPPER_ARG0(ls_t, escape, "--escape")

    CCSH_WRAPPER_ARG0(ls_t, N, "-N")   // --quoting-style=literal
    CCSH_WRAPPER_ARG0(ls_t, literal, "--literal")

    CCSH_WRAPPER_ARG0(ls_t, q, "-q")
    CCSH_WRAPPER_ARG0(ls_t, hide_control_chars, "--hide-control-chars")

    CCSH_WRAPPER_ARG0(ls_t, Q, "-Q")   // --quoting-style=c
    CCSH_WRAPPER_ARG0(ls_t, quote_name, "--quote-name")

    CCSH_WRAPPER_ARG1_E(ls_t, quoting_style, "--quoting-style", quoting_styles, base::enum_to_string(arg, quoting_styles_mapping))

};

template<typename T> constexpr const char * ls_t<T>::format_styles_mapping[];
template<typename T> constexpr const char * ls_t<T>::time_styles_mapping[];
template<typename T> constexpr const char * ls_t<T>::time_type_mapping[];
template<typename T> constexpr const char * ls_t<T>::sort_type_mapping[];
template<typename T> constexpr const char * ls_t<T>::quoting_styles_mapping[];
template<typename T> constexpr const char * ls_t<T>::indicator_styles_mapping[];
template<typename T> constexpr const char * ls_t<T>::color_type_mapping[];

template<typename T> constexpr typename ls_t<T>::none_t ls_t<T>::none;
template<typename T> constexpr typename ls_t<T>::locale_t ls_t<T>::locale;

}  // namespace hidden

using ls_t = hidden::ls_t<>;

using ls = command_holder<ls_t>;

}  // namespace ccsh

#endif // CCSH_CORE_LS_HPP_INCLUDED
