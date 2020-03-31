// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "CircleViewManagerCpp.h"
#include "CustomUserControlViewManagerCpp.h"
#include "SampleModuleCpp.h"

// Import generated definitions
#include "..\..\codegen\NativeModules.g.h"

// Import impls of modules
#include "MyModule.h"

using namespace winrt::Microsoft::ReactNative;

namespace SampleLibraryCpp {

// Lame that we need this right now
void RegisterModule(ReactModuleBuilder<::SampleLibraryCpp::MyModule> &moduleBuilder) {
  RegisterMyModuleModule(moduleBuilder);
}
}

namespace winrt::SampleLibraryCpp::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder);
  packageBuilder.AddModule(L"MyModule", MakeModuleProvider<::SampleLibraryCpp::MyModule>());
  packageBuilder.AddViewManager(
      L"CustomUserControlViewManagerCpp", []() { return winrt::make<CustomUserControlViewManagerCpp>(); });
  packageBuilder.AddViewManager(L"CircleViewManagerCpp", []() { return winrt::make<CircleViewManagerCpp>(); });
}

} // namespace winrt::SampleLibraryCpp::implementation
