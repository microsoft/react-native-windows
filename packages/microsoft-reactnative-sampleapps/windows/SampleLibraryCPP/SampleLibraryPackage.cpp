// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SampleLibraryPackage.h"
#if __has_include("SampleLibraryPackage.g.cpp")
#include "SampleLibraryPackage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Microsoft::ReactNative;

namespace winrt::SampleLibraryCPP::implementation {
IVectorView<Bridge::INativeModule> SampleLibraryPackage::CreateNativeModules(
    ReactContext const &reactContext) {
  if (nullptr == m_nativeModules) {
    auto modules = single_threaded_vector<Bridge::INativeModule>(
        {FancyMathABI(), SampleModuleABI()});
    m_nativeModules = modules.GetView();
  }

  return m_nativeModules;
}

IVectorView<Bridge::IViewManager> SampleLibraryPackage::CreateViewManagers(
    ReactContext const &reactContext) {
  if (nullptr == m_viewManagers) {
    auto viewManagers = single_threaded_vector<Bridge::IViewManager>();
    m_viewManagers = viewManagers.GetView();
  }

  return m_viewManagers;
}
} // namespace winrt::SampleLibraryCPP::implementation
