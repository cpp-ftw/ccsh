#include <ccsh/ccsh.hpp>
#include <ccsh/ccsh_runtime.hpp>
#include <ccsh/core.hpp>

using namespace ccsh;
using namespace ccsh::literals;
using namespace ccsh::core;

CCSH_RUN(ccsh::ps::prompt = []{
    return get_current_path_abbreviated().string() +
        " " +
        (ccsh::is_user_possibly_elevated() ? "# " : "$ ");
});
