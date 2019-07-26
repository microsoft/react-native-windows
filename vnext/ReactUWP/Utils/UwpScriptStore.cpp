#include "pch.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.h>
#include <future>
#include "Utils/UwpScriptStore.h"
#include "unicode.h"

namespace winrt {
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
} // namespace winrt

namespace react {
namespace uwp {

UwpScriptStore::UwpScriptStore() {}

/* static */ facebook::jsi::ScriptVersion_t UwpScriptStore::GetFileVersion(
    const std::wstring &filePath) {
  // append prefix to allow long file paths.
  auto longFilePath = L"\\\\?\\" + filePath;
  WIN32_FILE_ATTRIBUTE_DATA fileData;
  if (GetFileAttributesEx(
          longFilePath.c_str(), GetFileExInfoStandard, &fileData)) {
    return ((facebook::jsi::ScriptVersion_t)
                fileData.ftLastWriteTime.dwHighDateTime
            << 32) |
        fileData.ftLastWriteTime.dwLowDateTime;
  }

  return 0;
}

facebook::jsi::VersionedBuffer UwpScriptStore::getVersionedScript(
    const std::string &url) noexcept {
  facebook::jsi::VersionedBuffer versionedBuffer_;
  versionedBuffer_.buffer = nullptr;
  versionedBuffer_.version = 0;

  return versionedBuffer_;
}

// Script version = timestamp of bundle file last created
facebook::jsi::ScriptVersion_t UwpScriptStore::getScriptVersion(
    const std::string &url) noexcept {
  auto version = getScriptVersionAsync(url).get();

  return version;
}

std::future<facebook::jsi::ScriptVersion_t>
UwpScriptStore::getScriptVersionAsync(const std::string &bundleUri) {
  co_await winrt::resume_background();

  const winrt::hstring fileUrl(facebook::react::unicode::utf8ToUtf16(
      "ms-appx:///Bundle/" + bundleUri + ".bundle"));

  auto file = co_await winrt::StorageFile::GetFileFromApplicationUriAsync(
      winrt::Windows::Foundation::Uri{fileUrl});

  co_return GetFileVersion(file.Path().c_str());
}

} // namespace uwp
} // namespace react
