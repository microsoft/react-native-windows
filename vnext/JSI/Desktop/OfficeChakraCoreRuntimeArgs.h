// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "jsi/ScriptStore.h"

namespace facebook::jsi::chakra {

struct OfficeChakraCoreRuntimeArgs {
  bool m_asyncPreparedScriptGeneration = true;

  std::shared_ptr<ScriptStore> m_scriptStore;

  // m_preparedScriptStore->persistPreparedScript() must be synchronous.
  std::shared_ptr<PreparedScriptStore> m_preparedScriptStore;
};

} // namespace facebook::jsi::chakra
