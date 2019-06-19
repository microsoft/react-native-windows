#include "pch.h"
#include "UwpScriptStore.h"
#include <winrt/Windows.Storage.h>
#include <future>
#include "unicode.h"

namespace react { namespace uwp {

facebook::jsi::VersionedBuffer UwpScriptStore::getVersionedScript(const std::string& url) noexcept
{
  facebook::jsi::VersionedBuffer versionedBuffer_;
  versionedBuffer_.buffer = nullptr;
  versionedBuffer_.version = 0;

  return versionedBuffer_;
}

// Script version = timestamp of bundle file last created
facebook::jsi::ScriptVersion_t UwpScriptStore::getScriptVersion(const std::string& url) noexcept
{
  const std::string bundleUrl = "ms-appx:///Bundle/" + url + ".bundle";
  const winrt::Windows::Foundation::DateTime bundleCreatedTime = getBundleCreatedDate(bundleUrl).get();
  const std::uint64_t timestamp = bundleCreatedTime.time_since_epoch().count();
  return timestamp;
}

std::future<winrt::Windows::Foundation::DateTime> UwpScriptStore::getBundleCreatedDate(const std::string& bundleUri)
{
  winrt::hstring str(facebook::react::unicode::utf8ToUtf16(bundleUri));
  winrt::Windows::Foundation::Uri uri(str);

  co_await winrt::resume_background();

  try
  {
    auto file = co_await winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri);
    return file.DateCreated();
  }
  catch (winrt::hresult_error const& ex)
  {
    winrt::Windows::Foundation::DateTime date;
    return date;
  }
}

}}
