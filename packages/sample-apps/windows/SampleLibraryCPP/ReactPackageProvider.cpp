// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "CircleViewManagerCpp.h"
#include "CustomUserControlViewManagerCpp.h"
#include "SampleModuleCpp.h"

#include "MyJsiModule.h"
#include "MyModule.h"
#include "TurboModuleProvider.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::SampleLibraryCpp::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder, true);

  winrt::Microsoft::ReactNative::AddTurboModuleProvider<::SampleLibraryCpp::MyJsiModuleCxx>(
      packageBuilder, L"MyJsiModule");

  packageBuilder.AddViewManager(
      L"CustomUserControlViewManagerCpp", []() { return winrt::make<CustomUserControlViewManagerCpp>(); });
  packageBuilder.AddViewManager(L"CircleViewManagerCpp", []() { return winrt::make<CircleViewManagerCpp>(); });
}

} // namespace winrt::SampleLibraryCpp::implementation
