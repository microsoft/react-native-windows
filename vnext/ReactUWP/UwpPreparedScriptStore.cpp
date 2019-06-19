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

using namespace winrt::Windows;

namespace react { namespace uwp {
  UwpPreparedScriptStore::UwpPreparedScriptStore(winrt::hstring uri)
  {
    if (!uri.empty())
    {
      auto file = Storage::ApplicationData::Current().LocalFolder().TryGetItemAsync(uri).get();
      if (file)
      {
        m_byteCodeFileUri = uri;
      }
    }
  }

std::unique_ptr<const facebook::jsi::Buffer> UwpPreparedScriptStore::tryGetPreparedScript(
  const facebook::jsi::ScriptSignature& scriptSignature,
  const facebook::jsi::JSRuntimeSignature& runtimeSignature,
  const char* prepareTag // Optional tag. For e.g. eagerly evaluated vs lazy cache.
) noexcept
{
  // check if app bundle version is older than or equal to the prepared script version
  // if true then just read the buffer from the prepared script and return
  if (shouldGetPreparedScript(scriptSignature.version))
  {
    auto buffer = ByteCodeManager::ReadByteCodeFileAsync(m_byteCodeFileUri).get();
    auto bytecodeBuffer(std::make_unique<facebook::jsi::chakraruntime::ByteArrayBuffer>(buffer.Length()));
    auto dataReader{ Storage::Streams::DataReader::FromBuffer(buffer) };
    dataReader.ReadBytes(winrt::array_view<uint8_t> { &bytecodeBuffer->data()[0], &bytecodeBuffer->data()[bytecodeBuffer->size()] });
    dataReader.Close();

    return bytecodeBuffer;
  }
  return nullptr;
}

void UwpPreparedScriptStore::persistPreparedScript(
  std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
  const facebook::jsi::ScriptSignature& scriptMetadata,
  const facebook::jsi::JSRuntimeSignature& runtimeMetadata,
  const char* prepareTag  // Optional tag. For e.g. eagerly evaluated vs lazy cache.
) noexcept
{
  // generate a new bytecode file
  ByteCodeManager::CreateByteCodeFileAsync(preparedScript);
}

bool UwpPreparedScriptStore::shouldGetPreparedScript(facebook::jsi::ScriptVersion_t version) noexcept
{
  auto createdDateTime = ByteCodeManager::GetByteCodeFileCreatedDateAsync(m_byteCodeFileUri).get();
  std::uint64_t timestamp = createdDateTime.time_since_epoch().count();

  // try using default bytecode file url if given url is older than app bundle
  if (timestamp < version && !m_byteCodeFileUri.empty())
  {
    m_byteCodeFileUri = nullptr;
    return shouldGetPreparedScript(version);
  }

  return timestamp >= version;
}

std::future<Foundation::DateTime> ByteCodeManager::GetByteCodeFileCreatedDateAsync(winrt::hstring fileName)
{
  co_await winrt::resume_background();

  try
  {
    auto file = fileName.empty() ?
      co_await Storage::ApplicationData::Current().LocalCacheFolder().GetFileAsync(L"app.bytecode") :
      co_await Storage::StorageFile::GetFileFromApplicationUriAsync(Foundation::Uri(fileName));
    return file.DateCreated();
  }
  catch (winrt::hresult_error const& ex)
  {
    Foundation::DateTime date;
    return date;
  }
}

winrt::fire_and_forget ByteCodeManager::CreateByteCodeFileAsync(std::shared_ptr<const facebook::jsi::Buffer> preparedScript)
{
  auto folder = Storage::ApplicationData::Current().LocalCacheFolder();
  co_await winrt::resume_background();

  auto file = co_await folder.CreateFileAsync(L"app.bytecode", Storage::CreationCollisionOption::ReplaceExisting);
  Storage::FileIO::WriteBytesAsync(file, winrt::array_view<const uint8_t>{ &preparedScript->data()[0], &preparedScript->data()[preparedScript->size()] });
}

std::future<Storage::Streams::IBuffer> ByteCodeManager::ReadByteCodeFileAsync(winrt::hstring fileName)
{
  co_await winrt::resume_background();
  try
  {
    auto file = fileName.empty() ?
      co_await Storage::ApplicationData::Current().LocalCacheFolder().GetFileAsync(L"app.bytecode") :
      co_await Storage::StorageFile::GetFileFromApplicationUriAsync(Foundation::Uri(fileName));
    auto buffer = co_await Storage::FileIO::ReadBufferAsync(file);
    return buffer;
  }
  catch (winrt::hresult_error const& ex)
  {
    return nullptr;
  }
}

}}
