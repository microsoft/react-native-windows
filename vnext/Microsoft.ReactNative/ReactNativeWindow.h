#pragma once
#include "ReactNativeWindow.g.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactNativeWindow {
  ReactNativeWindow() = default;

  static winrt::Microsoft::ReactNative::ReactNativeHost CreateReactNativeHost(
      winrt::Microsoft::ReactNative::ReactApplicationInstanceSettings const &settings,
      winrt::Microsoft::UI::Windowing::AppWindow const &window,
      winrt::Microsoft::UI::Composition::Compositor const &compositor);

  static winrt::Microsoft::ReactNative::ReactNativeIsland CreateReactNativeIsland(
      winrt::Microsoft::UI::Composition::Compositor const &compositor,
      winrt::Microsoft::UI::Windowing::AppWindow const &window,
      winrt::Microsoft::ReactNative::ReactNativeHost const &host,
      hstring const &mainComponentName);
};
} // namespace winrt::Microsoft::ReactNative::implementation
namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactNativeWindow : ReactNativeWindowT<ReactNativeWindow, implementation::ReactNativeWindow> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
