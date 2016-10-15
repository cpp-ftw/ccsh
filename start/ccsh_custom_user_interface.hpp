#ifndef CCSH_CUSTOM_USER_INTERFACE_H_INCLUDED
#define CCSH_CUSTOM_USER_INTERFACE_H_INCLUDED


#include <cling/UserInterface/UserInterface.h>

namespace ccsh { namespace ui {

class custom_user_interface : protected cling::UserInterface
{
    using base = cling::UserInterface;

public:

    using base::base;
    using base::getMetaProcessor;

    void run_interactively();
};

} } // namespace ccsh::ui

#endif // CCSH_CUSTOM_USER_INTERFACE_H_INCLUDED