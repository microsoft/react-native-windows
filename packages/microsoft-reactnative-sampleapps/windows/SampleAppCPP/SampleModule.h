#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "SampleModule.g.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::SampleApp::implementation {

struct SampleModule : SampleModuleT<SampleModule> {
  SampleModule() = default;

  void method3(
      IVectorView<IInspectable> const &args,
      Bridge::Callback const &callback,
      Bridge::Callback const & /*ignored*/);

  hstring Name();
  IMapView<hstring, IInspectable> Constants();
  IVectorView<Bridge::MethodInfo> Methods();
  void Initialize();
};
} // namespace winrt::SampleApp::implementation

namespace winrt::SampleApp::factory_implementation {
struct SampleModule
    : SampleModuleT<SampleModule, implementation::SampleModule> {};
} // namespace winrt::SampleApp::factory_implementation
