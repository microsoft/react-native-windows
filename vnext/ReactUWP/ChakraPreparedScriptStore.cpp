#include "pch.h"
#include "ChakraPreparedScriptStore.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <Utils/LocalBundleReader.h>
#include "unicode.h"

using namespace winrt;

namespace react {
  namespace uwp {

    std::unique_ptr<const facebook::jsi::Buffer> ChakraPreparedScriptStore::tryGetPreparedScript(
      const facebook::jsi::ScriptSignature& scriptSignature,
      const facebook::jsi::JSRuntimeSignature& runtimeSignature,
      const char* prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
    ) noexcept
    {
      if (shouldGetPreparedScript(scriptSignature.version))
      {
        return getPreparedScriptAsync();
      }
      return nullptr;
    }

    void ChakraPreparedScriptStore::persistPreparedScript(
      std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
      const facebook::jsi::ScriptSignature& scriptMetadata,
      const facebook::jsi::JSRuntimeSignature& runtimeMetadata,
      const char* prepareTag  // Optional tag. For e.g. eagerly evaluated vs lazy cache.
    ) noexcept
    {
      persistPreparedScriptAsync(scriptMetadata.version, preparedScript);
    }

    bool ChakraPreparedScriptStore::shouldGetPreparedScript(facebook::jsi::ScriptVersion_t v) noexcept
    {
      const std::string bundleUrl = "ms-appx:///assets/app.bytecode";
      const winrt::Windows::Foundation::DateTime bytecodeFileCreatedTime = LocalBundleReader::LoadBundleCreatedDateTime(bundleUrl);
      const std::uint64_t timestamp = bytecodeFileCreatedTime.time_since_epoch().count();
        
      return timestamp > v;
    }

    std::unique_ptr<const facebook::jsi::Buffer> ChakraPreparedScriptStore::getPreparedScriptAsync() noexcept
    {
      return nullptr;
    }
    void ChakraPreparedScriptStore::persistPreparedScriptAsync(facebook::jsi::ScriptVersion_t v, std::shared_ptr<const facebook::jsi::Buffer>) noexcept
    {
      auto folder = Windows::Storage::ApplicationData::Current().LocalCacheFolder();

    }
  }
}
