#include "core.hpp"

namespace ccsh {
namespace core {

constexpr const char * tail_t::follow_type_mapping[];

constexpr const char * nl_t::numbering_style_mapping[];
constexpr const char * nl_t::numbering_format_mapping[];

constexpr const char * ls_t::format_styles_mapping[];
constexpr const char * ls_t::time_styles_mapping[];
constexpr const char * ls_t::time_type_mapping[];
constexpr const char * ls_t::sort_type_mapping[];
constexpr const char * ls_t::quoting_styles_mapping[];
constexpr const char * ls_t::indicator_styles_mapping[];
constexpr const char * ls_t::color_type_mapping[];

constexpr ls_t::none_t ls_t::none;
constexpr ls_t::locale_t ls_t::locale;

}
}
