#include "pch.h"
#include "Utils/UwpScriptStore.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.h>
#include <future>
#include "unicode.h"

namespace winrt {
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage;
} // namespace winrt

namespace react {
namespace uwp {

UwpScriptStore::UwpScriptStore() {}

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
  const std::string bundleUrl = "ms-appx:///Bundle/" + url + ".bundle";
  const winrt::DateTime bundleModifiedTime =
      getBundleModifiedDate(bundleUrl).get();
  const std::uint64_t timestamp = bundleModifiedTime.time_since_epoch().count();
  return timestamp;
}

std::future<winrt::DateTime> UwpScriptStore::getBundleModifiedDate(
    const std::string &bundleUri) {
  winrt::hstring str(facebook::react::unicode::utf8ToUtf16(bundleUri));
  winrt::Windows::Foundation::Uri uri(str);

  try {
    auto file =
        co_await winrt::StorageFile::GetFileFromApplicationUriAsync(uri);
    auto props = file.GetBasicPropertiesAsync().get();
    return props.DateModified();
  } catch (winrt::hresult_error const &ex) {
    winrt::DateTime date;
    return date;
  }
}

} // namespace uwp
} // namespace react
