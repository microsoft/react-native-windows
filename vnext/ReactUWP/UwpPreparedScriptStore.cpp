#include "pch.h"
#include "UwpPreparedScriptStore.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <Utils/LocalBundleReader.h>
#include "ChakraJsiRuntime.h"
#include <winrt/Windows.Storage.Streams.h>
#include "unicode.h"
#include "jsi/jsi.h"

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
  using namespace winrt::Windows::Foundation;
  using namespace winrt::Windows::Storage;
};

namespace react { namespace uwp {
  UwpPreparedScriptStore::UwpPreparedScriptStore(winrt::hstring uri)
  {
    if (!uri.empty())
    {
      m_byteCodeFileAsync = winrt::StorageFile::GetFileFromApplicationUriAsync(winrt::Uri(uri));
    }
  }

std::unique_ptr<const facebook::jsi::Buffer> UwpPreparedScriptStore::tryGetPreparedScript(
  const facebook::jsi::ScriptSignature& scriptSignature,
  const facebook::jsi::JSRuntimeSignature& runtimeSignature,
  const char* prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
) noexcept
{
  try {
    // check if app bundle version is older than or equal to the prepared script version
    // if true then just read the buffer from the prepared script and return


    auto byteCodeFile = TryGetByteCodeFileSync(scriptSignature.version);
    if (byteCodeFile == nullptr) {
      return nullptr;
    }

    auto buffer = winrt::FileIO::ReadBufferAsync(byteCodeFile).get();
    auto bytecodeBuffer(std::make_unique<facebook::jsi::chakraruntime::ByteArrayBuffer>(buffer.Length()));
    auto dataReader{ winrt::Streams::DataReader::FromBuffer(buffer) };
    dataReader.ReadBytes(winrt::array_view<uint8_t> { &bytecodeBuffer->data()[0], &bytecodeBuffer->data()[bytecodeBuffer->size()] });
    dataReader.Close();

    return bytecodeBuffer;
  }
  catch (...) {
    return nullptr;
  }
}

void UwpPreparedScriptStore::persistPreparedScript(
  std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
  const facebook::jsi::ScriptSignature& scriptMetadata,
  const facebook::jsi::JSRuntimeSignature& runtimeMetadata,
  const char* prepareTag  // Optional tag. For e.g. eagerly evaluated vs lazy cache.
) noexcept
{
  persistPreparedScriptAsync(preparedScript, scriptMetadata, runtimeMetadata, prepareTag);
}

winrt::fire_and_forget UwpPreparedScriptStore::persistPreparedScriptAsync(
  std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
  const facebook::jsi::ScriptSignature& scriptMetadata,
  const facebook::jsi::JSRuntimeSignature& runtimeMetadata,
  const char* prepareTag  // Optional tag. For e.g. eagerly evaluated vs lazy cache.
)
{
  try {
    co_await winrt::resume_background();
    auto folder = winrt::ApplicationData::Current().LocalCacheFolder();
    auto file = co_await folder.CreateFileAsync(L"app.bytecode", winrt::CreationCollisionOption::ReplaceExisting);
    winrt::FileIO::WriteBytesAsync(file, winrt::array_view<const uint8_t>{ &preparedScript->data()[0], &preparedScript->data()[preparedScript->size()] });
  }
  catch (...) {
  }
}

winrt::StorageFile UwpPreparedScriptStore::TryGetByteCodeFileSync(facebook::jsi::ScriptVersion_t version)
{
  try {
    if (m_byteCodeFileAsync != nullptr) {
      auto file = m_byteCodeFileAsync.get();
      auto createdDateTime = file.DateCreated();
      facebook::jsi::ScriptVersion_t byteCodeVersion = createdDateTime.time_since_epoch().count();
      if (byteCodeVersion >= version) {
        return file;
      }
    }
  }
  catch (...) {
    // Eat this exception. If we can't get the file from the uri. Still try looking  in the cache.
  }


  // Getting here means one of two things. No bytecode file uri was specified, or the file uri was specified but it is outdated.
  // Try looking in LocalCache folder for app.bytecode and use that.
  auto file = winrt::ApplicationData::Current().LocalCacheFolder().GetFileAsync(L"app.bytecode").get();
  auto createdDateTime = file.DateCreated();
  facebook::jsi::ScriptVersion_t byteCodeVersion = createdDateTime.time_since_epoch().count();

  return byteCodeVersion > version ? file : nullptr;
}
}}
