#include "pch.h"
#include "ChakraPreparedScriptStore.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <Utils/LocalBundleReader.h>
#include "LocalByteCodeManager.h"
#include "ChakraJsiRuntime.h"
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt;

namespace react {
  namespace uwp {
    std::unique_ptr<const facebook::jsi::Buffer> ChakraPreparedScriptStore::tryGetPreparedScript(
      const facebook::jsi::ScriptSignature& scriptSignature,
      const facebook::jsi::JSRuntimeSignature& runtimeSignature,
      const char* prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
    ) noexcept
    {
      // check if app bundle version is older than or equal to the prepared script version
      // if true then just read the buffer from the prepared script and return
      if (shouldGetPreparedScript(scriptSignature.version))
      {
        auto buffer = LocalByteCodeManager::LoadBuffer();
        std::unique_ptr<facebook::jsi::chakraruntime::ByteArrayBuffer> bytecodeBuffer(
          std::make_unique<facebook::jsi::chakraruntime::ByteArrayBuffer>(buffer.Length())
        );

        auto dataReader{ winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer) };
        std::vector<uint8_t> buffer_vector(buffer.Length());
        winrt::array_view<uint8_t> arrayView{ buffer_vector };
        dataReader.ReadBytes(arrayView);
        dataReader.Close();

        return bytecodeBuffer;
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
      // generate a new bytecode file
      LocalByteCodeManager::CreateFile(preparedScript.get()->data());
    }

    bool ChakraPreparedScriptStore::shouldGetPreparedScript(facebook::jsi::ScriptVersion_t v) noexcept
    {
      const winrt::Windows::Foundation::DateTime createdDateTime = LocalByteCodeManager::LoadCreatedDateTime();
      const std::uint64_t timestamp = createdDateTime.time_since_epoch().count();
      return timestamp >= v;
    }
  }
}
