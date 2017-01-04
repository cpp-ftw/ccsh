#ifndef CCSH_PS_HPP_INCLUDED
#define CCSH_PS_HPP_INCLUDED


#include "ccsh_tty.hpp"
#include "ccsh_utils.hpp"

#include "cling/Interpreter/Interpreter.h"

#define CCSH_RUN_CONCAT_HELPER(x, y) x ## y
#define CCSH_RUN_CONCAT(x, y) CCSH_RUN_CONCAT_HELPER(x, y)

#define CCSH_RUN(what) namespace ccsh { namespace hidden { int CCSH_RUN_CONCAT(run_helper_, __COUNTER__) = ((what), 0); }}

namespace ccsh {

std::string get_dollar_sign()
{
    static const char* normal_d = ccsh::is_user_possibly_elevated() ? "#" : "$";
    return gCling->isRawInputEnabled() ? "!" : normal_d;
}

}


#endif // CCSH_PS_HPP_INCLUDED
