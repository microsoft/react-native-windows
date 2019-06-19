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
  bool shouldGetPreparedScript(facebook::jsi::ScriptVersion_t v) noexcept;
  winrt::hstring m_byteCodeFileUri;
};

class ByteCodeManager
{
public:
  // Get byte code file created date to compare with app bundle file created date
  static std::future<winrt::Windows::Foundation::DateTime> GetByteCodeFileCreatedDateAsync(winrt::hstring fileName);

  // Create byte code file to re-use when app bundle did not change
  static winrt::fire_and_forget CreateByteCodeFileAsync(std::shared_ptr<const facebook::jsi::Buffer> preparedScript);

  // Read from byte code file
  static std::future<winrt::Windows::Storage::Streams::IBuffer> ReadByteCodeFileAsync(winrt::hstring fileName);
};

}}
