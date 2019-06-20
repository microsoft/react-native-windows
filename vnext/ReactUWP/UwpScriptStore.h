#pragma once
#include <jsi/ScriptStore.h>
#include <future>

namespace react { namespace uwp {

class UwpScriptStore : public facebook::jsi::ScriptStore
{
public:
  facebook::jsi::VersionedBuffer getVersionedScript(const std::string& url) noexcept override;
  facebook::jsi::ScriptVersion_t getScriptVersion(const std::string& url) noexcept override;
  UwpScriptStore();
  UwpScriptStore(const UwpScriptStore&) = delete;
  void operator=(const UwpScriptStore&) = delete;
private:
  std::future<winrt::Windows::Foundation::DateTime> getBundleModifiedDate(const std::string& bundlePath);
};

}}
