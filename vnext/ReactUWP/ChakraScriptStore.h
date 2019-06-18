#pragma once
#include <jsi/ScriptStore.h>

namespace react { namespace uwp {

class ChakraScriptStore : public facebook::jsi::ScriptStore
{
public:
  facebook::jsi::VersionedBuffer getVersionedScript(const std::string& url) noexcept override;
  facebook::jsi::ScriptVersion_t getScriptVersion(const std::string& url) noexcept override;
};

}}
