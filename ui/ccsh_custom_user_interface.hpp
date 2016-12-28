#ifndef CCSH_CUSTOM_USER_INTERFACE_H_INCLUDED
#define CCSH_CUSTOM_USER_INTERFACE_H_INCLUDED

#include <functional>

#include <cling/UserInterface/UserInterface.h>

namespace ccsh { namespace ui {

class custom_user_interface : protected cling::UserInterface
{
    using base = cling::UserInterface;

    std::string default_prompt();

public:

    custom_user_interface(cling::Interpreter& interp);
    using base::getMetaProcessor;

    void run_interactively();

    std::function<std::string()> prompt_factory;
};

}} // namespace ccsh::ui

#endif // CCSH_CUSTOM_USER_INTERFACE_H_INCLUDED
