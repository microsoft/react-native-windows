#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Bridge.NativeModuleBase.g.h"

using namespace winrt;
using namespace Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
struct NativeModuleBase : NativeModuleBaseT<NativeModuleBase> {
  NativeModuleBase() = default;

  hstring Name();
  IMapView<hstring, IInspectable> Constants();
  IVectorView<Bridge::MethodInfo> Methods();
  void Initialize();
};

} // namespace winrt::Microsoft::ReactNative::Bridge::implementation

namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation {

struct NativeModuleBase
    : NativeModuleBaseT<NativeModuleBase, implementation::NativeModuleBase> {};

} // namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation
