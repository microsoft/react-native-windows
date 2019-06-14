#pragma once
#include <jsi/ScriptStore.h>

namespace react {
  namespace uwp {
    class ChakraPreparedScriptStore : public facebook::jsi::PreparedScriptStore
    {
    public:
      std::unique_ptr<const facebook::jsi::Buffer> tryGetPreparedScript(
        const facebook::jsi::ScriptSignature& scriptSignature,
        const facebook::jsi::JSRuntimeSignature& runtimeSignature,
        const char* prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
      ) noexcept override;

      void persistPreparedScript(
        std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
        const facebook::jsi::ScriptSignature& scriptMetadata,
        const facebook::jsi::JSRuntimeSignature& runtimeMetadata,
        const char* prepareTag  // Optional tag. For e.g. eagerly evaluated vs lazy cache.
      ) noexcept override;
    };
  }
}
