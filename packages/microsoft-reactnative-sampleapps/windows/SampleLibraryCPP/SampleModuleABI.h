// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "SampleModuleABI.g.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;

namespace winrt::SampleLibraryCPP::implementation {
struct SampleModuleABI : SampleModuleABIT<SampleModuleABI> {
 public:
  SampleModuleABI() = default;

  // INativeModule

  hstring Name() {
    return L"SampleModuleABI";
  }

  void Initialize();

  IMapView<hstring, IInspectable> Constants();
  IVectorView<MethodInfo> Methods();

 private:
  IMapView<hstring, IInspectable> m_constants{nullptr};
  IVectorView<MethodInfo> m_methods{nullptr};
};
} // namespace winrt::SampleLibraryCPP::implementation

namespace winrt::SampleLibraryCPP::factory_implementation {
struct SampleModuleABI
    : SampleModuleABIT<SampleModuleABI, implementation::SampleModuleABI> {};
} // namespace winrt::SampleLibraryCPP::factory_implementation
