#include "pch.h"
#include "ChakraPreparedScriptStore.h"

namespace react {
  namespace uwp {

    std::unique_ptr<const facebook::jsi::Buffer> ChakraPreparedScriptStore::tryGetPreparedScript(
      const facebook::jsi::ScriptSignature& scriptSignature,
      const facebook::jsi::JSRuntimeSignature& runtimeSignature,
      const char* prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
    ) noexcept
    {
      return nullptr;
    }

    void ChakraPreparedScriptStore::persistPreparedScript(
      std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
      const facebook::jsi::ScriptSignature& scriptMetadata,
      const facebook::jsi::JSRuntimeSignature& runtimeMetadata,
      const char* prepareTag  // Optional tag. For e.g. eagerly evaluated vs lazy cache.
    ) noexcept
    {

    }
  }
}
