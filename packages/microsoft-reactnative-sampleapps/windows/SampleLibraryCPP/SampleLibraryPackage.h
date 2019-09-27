// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "SampleLibraryPackage.g.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::SampleLibraryCPP::implementation {
struct SampleLibraryPackage : SampleLibraryPackageT<SampleLibraryPackage> {
  SampleLibraryPackage() = default;

  IVectorView<INativeModule> CreateNativeModules(
      ReactContext const &reactContext);

  IVectorView<IViewManager> CreateViewManagers(
      ReactContext const &reactContext);
};
} // namespace winrt::SampleLibraryCPP::implementation

namespace winrt::SampleLibraryCPP::factory_implementation {
struct SampleLibraryPackage : SampleLibraryPackageT<
                                  SampleLibraryPackage,
                                  implementation::SampleLibraryPackage> {};
} // namespace winrt::SampleLibraryCPP::factory_implementation
