#pragma once
#include <JSI/ScriptStore.h>
#include <cxxreact/JSBigString.h>
#include <jsi/jsi.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <future>
#include <string>

namespace react::uwp {
class UwpPreparedScriptStore : public facebook::jsi::PreparedScriptStore {
 public:
  UwpPreparedScriptStore(winrt::hstring uri);
  std::shared_ptr<const facebook::jsi::Buffer> tryGetPreparedScript(
      const facebook::jsi::ScriptSignature &scriptSignature,
      const facebook::jsi::JSRuntimeSignature &runtimeSignature,
      const char *prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
      ) noexcept override;

  void persistPreparedScript(
      std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
      const facebook::jsi::ScriptSignature &scriptMetadata,
      const facebook::jsi::JSRuntimeSignature &runtimeMetadata,
      const char *prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
      ) noexcept override;

  UwpPreparedScriptStore(const UwpPreparedScriptStore &) = delete;
  void operator=(const UwpPreparedScriptStore &) = delete;

 private:
  winrt::fire_and_forget persistPreparedScriptAsync(
      std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
      const facebook::jsi::ScriptSignature &scriptMetadata,
      const facebook::jsi::JSRuntimeSignature &runtimeMetadata,
      const char *prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
  );
  winrt::Windows::Storage::StorageFile TryGetByteCodeFileSync(const facebook::jsi::ScriptSignature &scriptSignature);
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> m_byteCodeFileAsync;
};

// This is very similiar to ByteArrayBuffer in ChakraJsiRuntime.h.
// Defining this to avoid referencing types in chakra headers
class ByteCodeBuffer final : public facebook::jsi::Buffer {
 public:
  size_t size() const override {
    return size_;
  }
  const uint8_t *data() const {
    return byteArray_.get();
  }

  uint8_t *data() {
    return byteArray_.get();
  }

  ByteCodeBuffer(int size) : size_(size), byteArray_(std::make_unique<uint8_t[]>(size)) {}
  ByteCodeBuffer(const ByteCodeBuffer &) = delete;
  void operator=(const ByteCodeBuffer &) = delete;

 private:
  int size_;
  std::unique_ptr<uint8_t[]> byteArray_;
};
} // namespace react::uwp
