// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ModuleRegistration.h"

namespace winrt::Microsoft::ReactNative {

const ModuleRegistration *ModuleRegistration::s_head{nullptr};

ModuleRegistration::ModuleRegistration(const wchar_t *moduleName) noexcept : m_moduleName{moduleName}, m_next{s_head} {
  s_head = this;
}

void AddAttributedModules(IReactPackageBuilder const &packageBuilder) noexcept {
  for (const ModuleRegistration *reg = ModuleRegistration::Head(); reg != nullptr; reg = reg->Next()) {
    packageBuilder.AddModule(winrt::to_hstring(reg->ModuleName()), reg->MakeModuleProvider());
  }
}

} // namespace winrt::Microsoft::ReactNative
