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

using namespace winrt::Microsoft::ReactNative;

namespace winrt::SampleLibraryCpp::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder);
  packageBuilder.AddViewManager(L"CustomUserControlViewManagerCpp", [](IReactContext const &reactContext) {
    return winrt::make<CustomUserControlViewManagerCpp>(reactContext);
  });
  packageBuilder.AddViewManager(L"CircleViewManagerCpp", [](IReactContext const & /*reactContext*/) {
    return winrt::make<CircleViewManagerCpp>();
  });
}

} // namespace winrt::SampleLibraryCpp::implementation
