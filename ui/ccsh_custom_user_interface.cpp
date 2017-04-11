//------------------------------------------------------------------------------
// CLING - the C++ LLVM-based InterpreterG :)
// author:  Axel Naumann <axel@cern.ch>
//
// This file is dual-licensed: you can choose to license it under the University
// of Illinois Open Source License or the GNU Lesser General Public License. See
// LICENSE.TXT for details.
//------------------------------------------------------------------------------

#include "ccsh_custom_user_interface.hpp"

#include "cling/Interpreter/Exception.h"
#include "cling/Interpreter/Interpreter.h"
#include "cling/MetaProcessor/MetaProcessor.h"
#include "cling/Utils/Output.h"
#include "textinput/Callbacks.h"
#include "textinput/TextInput.h"
#include "textinput/StreamReader.h"
#include "textinput/TerminalDisplay.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/CompilerInstance.h"

#include <ccsh/ccsh_utils.hpp>

// Fragment copied from LLVM's raw_ostream.cpp
#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif

#if defined(_MSC_VER)
#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif
#endif

#include <memory>
#include <iostream>


namespace {
///\brief Class that specialises the textinput TabCompletion to allow Cling
/// to code complete through its own textinput mechanism which is part of the
/// UserInterface.
///
class UITabCompletion : public textinput::TabCompletion
{
    const cling::Interpreter& m_ParentInterpreter;

public:
    UITabCompletion(const cling::Interpreter& Parent)
        : m_ParentInterpreter(Parent)
    { }

    ~UITabCompletion()
    { }

    bool Complete(textinput::Text& Line /*in+out*/,
                  std::size_t& Cursor /*in+out*/,
                  textinput::EditorRange& R /*out*/,
                  std::vector<std::string>& Completions /*out*/) override
    {
        m_ParentInterpreter.codeComplete(Line.GetText(), Cursor, Completions);
        return true;
    }
};
}


namespace ccsh { namespace ui {

custom_user_interface::custom_user_interface(cling::Interpreter& interp)
    : base(interp)
    , prompt_factory(std::bind(&custom_user_interface::default_prompt, this))
{

}


std::string custom_user_interface::default_prompt()
{
    static const char* prompt_dollar = ccsh::is_user_possibly_elevated() ? "# " : "$ ";

    std::string prompt = "[ccsh]";
    if (getMetaProcessor()->getInterpreter().isRawInputEnabled())
        prompt.append("! ");
    else
        prompt.append(prompt_dollar);

    return prompt;
}


void custom_user_interface::run_interactively()
{

    std::string histfilePath = (ccsh::get_home() / ".cling_history").string();

    using namespace textinput;
    std::unique_ptr<StreamReader> R(StreamReader::Create());
    std::unique_ptr<TerminalDisplay> D(TerminalDisplay::Create());
    TextInput TI(*R, *D, histfilePath.c_str());

    // Inform text input about the code complete consumer
    // TextInput owns the TabCompletion.
    TI.SetCompletion(new UITabCompletion(getMetaProcessor()->getInterpreter()));

    TI.SetPrompt(prompt_factory().c_str());
    std::string line;
    std::string prompt = prompt_factory();
    while (true)
    {
        try
        {
            {
                cling::MetaProcessor::MaybeRedirectOutputRAII RAII(*getMetaProcessor());
                TI.SetPrompt(prompt.c_str());
                if (TI.ReadInput() == TextInput::kRREOF)
                    break;
                TI.TakeInput(line);
            }

            cling::Interpreter::CompilationResult compRes;
            int indent = getMetaProcessor()->process(line.c_str(), compRes);
            // Quit requested
            if (indent < 0)
                break;

            prompt = prompt_factory();
            if (indent > 0) // Continuation requested.
                prompt += '?' + std::string(indent * 3, ' ');

            std::cout.flush();
            std::cerr.flush();
        }
        catch (cling::InterpreterException& e)
        {
            if (!e.diagnose())
            {
                cling::errs() << ">>> Caught an interpreter exception!\n"
                              << ">>> " << e.what() << '\n';
            }
        }
        catch (std::exception& e)
        {
            cling::errs() << ">>> Caught a std::exception!\n"
                          << ">>> " << e.what() << '\n';
        }
        catch (...)
        {
            cling::errs() << "Exception occurred. Recovering...\n";
        }
    }
}

}} // namespace ccsh::ui

