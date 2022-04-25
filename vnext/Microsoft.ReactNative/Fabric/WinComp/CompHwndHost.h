// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "CompHwndHost.g.h"

#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include "ReactHost/React.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct CompHwndHost : CompHwndHostT<CompHwndHost> {
  CompHwndHost() noexcept;

  uint64_t HWnd() noexcept;

  void Initialize(uint64_t hwnd) noexcept;

  // property ReactNativeHost
  ReactNative::ReactNativeHost ReactNativeHost() noexcept;
  void ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept;

  // property ComponentName
  hstring ComponentName() noexcept;
  void ComponentName(hstring const &value) noexcept;

  // property InitialProps
  ReactNative::JSValueArgWriter InitialProps() noexcept;
  void InitialProps(ReactNative::JSValueArgWriter const &value) noexcept;

  void ReloadView() noexcept;
  winrt::Windows::UI::Composition::Compositor Compositor() const noexcept;
  void Compositor(winrt::Windows::UI::Composition::Compositor const &value) noexcept;
  winrt::Windows::UI::Composition::Visual RootVisual() const noexcept;

  LRESULT TranslateMessage(int msg, WPARAM wParam, LPARAM lParam) noexcept;

 private:
  // Possibly should be public..
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget Target() const noexcept;

  void EnsureTarget() noexcept;
  void CreateDesktopWindowTarget(HWND window);
  void CreateCompositionRoot();
  void UpdateSize() noexcept;
  double ScaleFactor() noexcept;

  HWND m_hwnd;
  winrt::Microsoft::ReactNative::CompRootView m_compRootView{nullptr};
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{nullptr};

  // Store locally if set before we have a rootview
  hstring m_componentName;
  winrt::Windows::UI::Composition::Compositor m_compositor{nullptr};
  ReactNative::ReactNativeHost m_reactNativeHost{nullptr};
  ReactNative::JSValueArgWriter m_initialPropsWriter{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct CompHwndHost : CompHwndHostT<CompHwndHost, implementation::CompHwndHost> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
