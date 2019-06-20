#pragma once
#include <jsi/ScriptStore.h>
#include <winrt/Windows.Foundation.h>
#include <cxxreact/JSBigString.h>
#include <string>
#include <future>
#include <winrt/Windows.Storage.h>
#include "jsi/jsi.h"

namespace react { namespace uwp {
class UwpPreparedScriptStore : public facebook::jsi::PreparedScriptStore
{
public:
  UwpPreparedScriptStore(winrt::hstring uri);
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
private:
  winrt::fire_and_forget persistPreparedScriptAsync(
    std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
    const facebook::jsi::ScriptSignature& scriptMetadata,
    const facebook::jsi::JSRuntimeSignature& runtimeMetadata,
    const char* prepareTag  // Optional tag. For e.g. eagerly evaluated vs lazy cache.
  ) noexcept;
  winrt::Windows::Storage::StorageFile TryGetByteCodeFileSync(facebook::jsi::ScriptVersion_t version);
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> m_byteCodeFileAsync;
};
}}
