#include "pch.h"
#include "ChakraScriptStore.h"

namespace react {
  namespace uwp {

    facebook::jsi::VersionedBuffer ChakraScriptStore::getVersionedScript(const std::string& url) noexcept
    {
      facebook::jsi::VersionedBuffer versionedBuffer_;
      versionedBuffer_.buffer = 0;
      versionedBuffer_.version = 0;

      return versionedBuffer_;
    }

    facebook::jsi::ScriptVersion_t ChakraScriptStore::getScriptVersion(const std::string& url) noexcept
    {
      facebook::jsi::ScriptVersion_t scriptVersion_ = 0;

      return scriptVersion_;
    }
  }
}
