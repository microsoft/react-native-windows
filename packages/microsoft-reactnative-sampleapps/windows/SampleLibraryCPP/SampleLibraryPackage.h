// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "SampleLibraryPackage.g.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;

namespace winrt::SampleLibraryCPP::implementation {
struct SampleLibraryPackage : SampleLibraryPackageT<SampleLibraryPackage> {
  SampleLibraryPackage() = default;

  IVectorView<INativeModule> CreateNativeModules(
      ReactContext const &reactContext);

  IVectorView<IViewManager> CreateViewManagers(
      ReactContext const &reactContext);

 private:
  IVectorView<INativeModule> m_nativeModules{nullptr};
  IVectorView<IViewManager> m_viewManagers{nullptr};
};
} // namespace winrt::SampleLibraryCPP::implementation

namespace winrt::SampleLibraryCPP::factory_implementation {
struct SampleLibraryPackage : SampleLibraryPackageT<
                                  SampleLibraryPackage,
                                  implementation::SampleLibraryPackage> {};
} // namespace winrt::SampleLibraryCPP::factory_implementation
