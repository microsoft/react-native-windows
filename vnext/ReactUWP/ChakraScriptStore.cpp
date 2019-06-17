#include "pch.h"
#include "ChakraScriptStore.h"
#include <winrt/Windows.Storage.h>
#include <Utils/LocalBundleReader.h>

namespace react {
  namespace uwp {

    facebook::jsi::VersionedBuffer ChakraScriptStore::getVersionedScript(const std::string& url) noexcept
    {
      facebook::jsi::VersionedBuffer versionedBuffer_;
      versionedBuffer_.buffer = 0;
      versionedBuffer_.version = 0;

      return versionedBuffer_;
    }

    // Script version = timestamp of bundle file last created
    facebook::jsi::ScriptVersion_t ChakraScriptStore::getScriptVersion(const std::string& url) noexcept
    {
      const std::string bundleUrl = "ms-appx:///Bundle/App.bundle";
      const winrt::Windows::Foundation::DateTime bundleCreatedTime = LocalBundleReader::LoadBundleCreatedDateTime(bundleUrl);
      const std::uint64_t timestamp = bundleCreatedTime.time_since_epoch().count();
      facebook::jsi::ScriptVersion_t scriptVersion_ = timestamp;

      return scriptVersion_;
    }
  }
}
