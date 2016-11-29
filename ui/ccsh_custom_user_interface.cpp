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
#include "cling/MetaProcessor/MetaProcessor.h"
#include "textinput/Callbacks.h"
#include "textinput/TextInput.h"
#include "textinput/StreamReader.h"
#include "textinput/TerminalDisplay.h"

#include "llvm/Support/raw_ostream.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/CompilerInstance.h"

#include <ccsh/ccsh_utils.hpp>

// Fragment copied from LLVM's raw_ostream.cpp
#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif

#if defined(LLVM_ON_WIN32)
#include <Shlobj.h>
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
  class UITabCompletion : public textinput::TabCompletion {
    const cling::Interpreter& m_ParentInterpreter;

  public:
    UITabCompletion(const cling::Interpreter& Parent) :
                    m_ParentInterpreter(Parent) {}
    ~UITabCompletion() {}

    bool Complete(textinput::Text& Line /*in+out*/,
                  std::size_t& Cursor /*in+out*/,
                  textinput::EditorRange& R /*out*/,
                  std::vector<std::string>& Completions /*out*/) override {
      m_ParentInterpreter.codeComplete(Line.GetText(), Cursor, Completions);
      return true;
    }
  };
}


namespace ccsh { namespace ui {

void custom_user_interface::run_interactively() {

  std::string histfilePath = (ccsh::get_home() / ".cling_history").string();

  using namespace textinput;
  std::unique_ptr<StreamReader> R(StreamReader::Create());
  std::unique_ptr<TerminalDisplay> D(TerminalDisplay::Create());
  TextInput TI(*R, *D, histfilePath.empty() ? nullptr : histfilePath.c_str());

  // Inform text input about the code complete consumer
  // TextInput owns the TabCompletion.
  TI.SetCompletion(new UITabCompletion(getMetaProcessor()->getInterpreter()));

  const char * prompt_dollar = ccsh::is_user_possibly_elevated() ? "# " : "$ ";

  TI.SetPrompt((std::string("[ccsh]") + prompt_dollar).c_str());
  std::string line;
  while (true) {
    try {
      getMetaProcessor()->getOuts().flush();
      TextInput::EReadResult RR = TI.ReadInput();
      TI.TakeInput(line);
      if (RR == TextInput::kRREOF) {
        break;
      }

      cling::Interpreter::CompilationResult compRes;
      cling::MetaProcessor::MaybeRedirectOutputRAII RAII(getMetaProcessor());
      int indent = getMetaProcessor()->process(line.c_str(), compRes, 0/*result*/);
      // Quit requested
      if (indent < 0)
        break;
      std::string Prompt = "[ccsh]";
      if (getMetaProcessor()->getInterpreter().isRawInputEnabled())
        Prompt.append("! ");
      else
        Prompt.append(prompt_dollar);

      if (indent > 0)
        // Continuation requested.
        Prompt.append('?' + std::string(indent * 3, ' '));

      TI.SetPrompt(Prompt.c_str());

    }
    catch(cling::InvalidDerefException& e) {
      e.diagnose();
    }
    catch(cling::InterpreterException& e) {
      std::cerr << ">>> Caught an interpreter exception!\n"
                   << ">>> " << e.what() << '\n';
    }
    catch(std::exception& e) {
      std::cerr << ">>> Caught a std::exception!\n"
                   << ">>> " << e.what() << '\n';
    }
    catch(...) {
      std::cerr << "Exception occurred. Recovering...\n";
    }
  }
}

} } // namespace ccsh::ui

