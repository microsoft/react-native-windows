#pragma once
#include <jsi/ScriptStore.h>
#include <future>

namespace react {
namespace uwp {

class UwpScriptStore : public facebook::jsi::ScriptStore {
 public:
  facebook::jsi::VersionedBuffer getVersionedScript(
      const std::string &url) noexcept override;
  facebook::jsi::ScriptVersion_t getScriptVersion(
      const std::string &url) noexcept override;
  UwpScriptStore();
  UwpScriptStore(const UwpScriptStore &) = delete;
  void operator=(const UwpScriptStore &) = delete;

 public:
  static facebook::jsi::ScriptVersion_t GetFileVersion(
      const std::wstring &filePath);

 private:
  std::future<facebook::jsi::ScriptVersion_t> getScriptVersionAsync(
      const std::string &bundleUri);
};

} // namespace uwp
} // namespace react
