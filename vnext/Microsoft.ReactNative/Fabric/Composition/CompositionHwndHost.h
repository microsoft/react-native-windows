// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "CompositionHwndHost.g.h"

#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include "ReactHost/React.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct CompositionHwndHost : CompositionHwndHostT<CompositionHwndHost> {
  CompositionHwndHost() noexcept;

  uint64_t HWnd() noexcept;

  void Initialize(uint64_t hwnd) noexcept;

  winrt::Microsoft::ReactNative::IReactViewHost ReactViewHost() const noexcept;
  void ReactViewHost(winrt::Microsoft::ReactNative::IReactViewHost const &value) noexcept;

  winrt::Windows::UI::Composition::Visual RootVisual() const noexcept;

  LRESULT TranslateMessage(int msg, uint64_t wParam, int64_t lParam) noexcept;

 private:
  // Possibly should be public..
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget Target() const noexcept;
  winrt::Windows::UI::Composition::Compositor Compositor() const noexcept;

  void CreateDesktopWindowTarget(HWND window);
  void CreateCompositionRoot();
  void UpdateSize() noexcept;
  double ScaleFactor() noexcept;

  HWND m_hwnd;
  winrt::Microsoft::ReactNative::CompositionRootView m_compRootView{nullptr};
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{nullptr};

  // Store locally if set before we have a rootview
  ReactNative::IReactViewHost m_reactViewHost{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct CompositionHwndHost : CompositionHwndHostT<CompositionHwndHost, implementation::CompositionHwndHost> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
