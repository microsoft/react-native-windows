// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "ReactNativeAppBuilder.g.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder> {
  ReactNativeAppBuilder();

  ~ReactNativeAppBuilder();

  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetDispatcherQueueController(
      winrt::Microsoft::UI::Dispatching::DispatcherQueueController const &dispatcherQueueController);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetCompositor(
      winrt::Microsoft::UI::Composition::Compositor const &compositor);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetAppWindow(
      winrt::Microsoft::UI::Windowing::AppWindow const &appWindow);
  winrt::Microsoft::ReactNative::ReactNativeWin32App Build();

 private:
  winrt::Microsoft::ReactNative::ReactNativeWin32App m_reactNativeWin32App{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation
namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder, implementation::ReactNativeAppBuilder> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
