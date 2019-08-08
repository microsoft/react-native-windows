#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Bridge.ReactContext.g.h"

#include "ReactInstance.h"

using namespace winrt;
using namespace Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
struct ReactContext : ReactContextT<ReactContext> {
  ReactContext() = default;

  IReactInstance ReactInstance();

  void InitializeWithInstance(Bridge::ReactInstance const &instance);

  void CallJSFunction(
      hstring const &moduleName,
      hstring const &method,
      IVectorView<IInspectable> const &params);

 private:
  Bridge::ReactInstance m_reactInstance{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::Bridge::implementation

namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation {
struct ReactContext
    : ReactContextT<ReactContext, implementation::ReactContext> {};
} // namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation
