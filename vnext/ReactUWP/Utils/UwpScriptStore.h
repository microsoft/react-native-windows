#pragma once
#include <JSI/Shared/ScriptStore.h>
#include <future>

namespace react {
namespace uwp {

class UwpScriptStore : public Microsoft::JSI::ScriptStore {
 public:
  Microsoft::JSI::VersionedBuffer getVersionedScript(const std::string &url) noexcept override;
  Microsoft::JSI::ScriptVersion_t getScriptVersion(const std::string &url) noexcept override;
  UwpScriptStore();
  UwpScriptStore(const UwpScriptStore &) = delete;
  void operator=(const UwpScriptStore &) = delete;

 public:
  static Microsoft::JSI::ScriptVersion_t GetFileVersion(const std::wstring &filePath);

 private:
  std::future<Microsoft::JSI::ScriptVersion_t> getScriptVersionAsync(const std::string &bundleUri);
};

} // namespace uwp
} // namespace react
