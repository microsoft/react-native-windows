#pragma once
#include "ReactNativeIsland.g.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactNativeIsland : ReactNativeIslandT<ReactNativeIsland> {
  ReactNativeIsland() = default;

  ReactNativeIsland(
      winrt::Microsoft::UI::Composition::Compositor const &compositor,
      winrt::Microsoft::UI::Windowing::AppWindow const &window,
      winrt::Microsoft::ReactNative::ReactNativeHost const &host,
      winrt::hstring const &mainComponentName);

  winrt::Microsoft::ReactNative::CompositionRootView RootView() {
    return m_rootView;
  }

 private:
  winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Microsoft::ReactNative::CompositionRootView m_rootView{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation
