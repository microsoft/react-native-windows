// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DevSettings.h"

#include <cxxreact/JSBigString.h>
#include <string>

namespace facebook {
namespace react {

// Used to store and retrive bytecode. Superseded in JSI by PreparedScriptStore.
class ChakraBytecodeStore {
 public:
  ChakraBytecodeStore(const std::shared_ptr<ScriptPropertyResolver> &resolver);

  // Wil return bytecode for the Url if it exists may be reused
  std::unique_ptr<JSBigString> tryReadExistingBytecode(const std::string &scriptUrl);

  // Attempt to store bytecode for the given script (not guranteed to succeed)
  void persistBytecode(const std::string &scriptUrl, const JSBigString &bytecode);

 private:
  std::shared_ptr<ScriptPropertyResolver> m_resolver;
};

} // namespace react
} // namespace facebook
