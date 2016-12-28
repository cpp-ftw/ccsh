#ifndef CCSH_PS_HPP_INCLUDED
#define CCSH_PS_HPP_INCLUDED


#include "ccsh_tty.hpp"

#define CCSH_RUN_CONCAT_HELPER(x, y) x ## y
#define CCSH_RUN_CONCAT(x, y) CCSH_RUN_CONCAT_HELPER(x, y)

#define CCSH_RUN(what) namespace ccsh { namespace hidden { int CCSH_RUN_CONCAT(run_helper_, __COUNTER__) = ((what), 0); }}


#endif // CCSH_PS_HPP_INCLUDED
