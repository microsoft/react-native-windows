#include "pch.h"
#include "ModuleRegistration.h"

namespace Microsoft::ReactNative {

const ModuleRegistration *ModuleRegistration::s_head{nullptr};

ModuleRegistration::ModuleRegistration(
    const char *moduleName,
    const char *eventEmitterName) noexcept
    : m_moduleName{moduleName},
      m_eventEmitterName{eventEmitterName},
      m_next{s_head} {
  s_head = this;
}

void AddAttributedModules(
    winrt::Microsoft::ReactNative::Bridge::IReactPackageBuilder const
        &packageBuilder) noexcept {
  for (const ModuleRegistration *reg = ModuleRegistration::Head();
       reg != nullptr;
       reg = reg->Next()) {
    packageBuilder.AddModule(
        winrt::to_hstring(reg->ModuleName()), reg->MakeModuleProvider());
  }
}

} // namespace Microsoft::ReactNative
