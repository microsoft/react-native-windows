#include "pch.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.h>
#include <future>
#include "Unicode.h"
#include "Utils/UwpScriptStore.h"

namespace winrt {
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
} // namespace winrt

namespace react {
namespace uwp {

UwpScriptStore::UwpScriptStore() {}

/* static */ Microsoft::JSI::ScriptVersion_t UwpScriptStore::GetFileVersion(const std::wstring &filePath) {
  // append prefix to allow long file paths.
  auto longFilePath = L"\\\\?\\" + filePath;
  WIN32_FILE_ATTRIBUTE_DATA fileData;
  if (GetFileAttributesEx(longFilePath.c_str(), GetFileExInfoStandard, &fileData)) {
    return ((Microsoft::JSI::ScriptVersion_t)fileData.ftLastWriteTime.dwHighDateTime << 32) |
        fileData.ftLastWriteTime.dwLowDateTime;
  }

  return 0;
}

Microsoft::JSI::VersionedBuffer UwpScriptStore::getVersionedScript(const std::string &url) noexcept {
  Microsoft::JSI::VersionedBuffer versionedBuffer_;
  versionedBuffer_.buffer = nullptr;
  versionedBuffer_.version = 0;

  return versionedBuffer_;
}

// Script version = timestamp of bundle file last created
Microsoft::JSI::ScriptVersion_t UwpScriptStore::getScriptVersion(const std::string &url) noexcept {
  auto version = getScriptVersionAsync(url).get();

  return version;
}

std::future<Microsoft::JSI::ScriptVersion_t> UwpScriptStore::getScriptVersionAsync(const std::string &bundleUri) {
  co_await winrt::resume_background();

  const winrt::hstring fileUrl(Microsoft::Common::Unicode::Utf8ToUtf16("ms-appx:///Bundle/" + bundleUri + ".bundle"));

  auto file = co_await winrt::StorageFile::GetFileFromApplicationUriAsync(winrt::Windows::Foundation::Uri{fileUrl});

  co_return GetFileVersion(file.Path().c_str());
}

} // namespace uwp
} // namespace react
