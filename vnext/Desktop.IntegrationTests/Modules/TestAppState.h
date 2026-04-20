// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <codegen/NativeAppStateSpec.g.h>
#include <NativeModules.h>

namespace Microsoft::React::Test {

REACT_MODULE(AppState)
struct AppState {
  using ModuleSpec = ::Microsoft::ReactNativeSpecs::AppStateSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &) noexcept;

  REACT_METHOD(GetCurrentAppState, L"getCurrentAppState")
  void GetCurrentAppState(
      std::function<void(::Microsoft::ReactNativeSpecs::AppStateSpec_AppState const &)> const &success,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &) noexcept;

  REACT_METHOD(AddListener, L"addListener")
  void AddListener(std::string) noexcept;

  REACT_METHOD(RemoveListeners, L"removeListeners")
  void RemoveListeners(double) noexcept;

  REACT_GET_CONSTANTS(GetConstants)
  ::Microsoft::ReactNativeSpecs::AppStateSpec_AppStateConstants GetConstants() noexcept;
};

} // namespace Microsoft::React::Test
