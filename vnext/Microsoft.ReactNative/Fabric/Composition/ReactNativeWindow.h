// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "ReactNativeWindow.g.h"

#include <ReactContext.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactNativeWindow
    : ReactNativeWindowT<ReactNativeWindow> {
  ~ReactNativeWindow() noexcept;
  ReactNativeWindow() noexcept;

  static winrt::Microsoft::ReactNative::ReactNativeWindow CreateFromCompositor(const winrt::Microsoft::UI::Composition::Compositor& compositor) noexcept;
  static winrt::Microsoft::ReactNative::ReactNativeWindow CreateFromWindow(const winrt::Microsoft::UI::Windowing::AppWindow& appWindow, const winrt::Microsoft::UI::Composition::Compositor& compositor) noexcept;
  static winrt::Microsoft::ReactNative::ReactNativeWindow CreateFromWindowAndIsland(const winrt::Microsoft::UI::Windowing::AppWindow& appWindow, const winrt::Microsoft::ReactNative::ReactNativeIsland& island) noexcept;
  static winrt::Microsoft::ReactNative::ReactNativeWindow CreateFromContentSiteBridgeAndIsland(const winrt::Microsoft::UI::Content::IContentSiteBridge& bridge, const winrt::Microsoft::ReactNative::ReactNativeIsland& island);

  winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeIsland() noexcept;

  winrt::Microsoft::UI::Windowing::AppWindow AppWindow() noexcept;

  winrt::Microsoft::UI::Content::IContentSiteBridge ContentSiteBridge() noexcept;

  winrt::Microsoft::ReactNative::ContentSizePolicy ResizePolicy() const noexcept;
  void ResizePolicy(winrt::Microsoft::ReactNative::ContentSizePolicy value) noexcept;

  winrt::Microsoft::ReactNative::LayoutConstraints LayoutConstraints() const noexcept;
  void LayoutConstraints(winrt::Microsoft::ReactNative::LayoutConstraints const &value) noexcept;

  void Close() noexcept;

 private:

   void Connect(winrt::Microsoft::UI::Content::ContentIsland contentIsland) noexcept;
  winrt::Microsoft::UI::Composition::Compositor Compositor() noexcept;
  void AppWindow(const winrt::Microsoft::UI::Windowing::AppWindow &appWindow ) noexcept;
  void AppWindow_Changed(
                      winrt::Microsoft::UI::Windowing::AppWindow const &window,
                      winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const &args) noexcept;
  
  void ReactNativeIsland(const winrt::Microsoft::ReactNative::ReactNativeIsland &value) noexcept;

  void UpdateIslandScaleFactor() noexcept;
  void EnsureResizePolicyState() noexcept;
  void UpdateRootViewSizeToAppWindow(
    winrt::Microsoft::ReactNative::ReactNativeIsland const &rootView,
    winrt::Microsoft::UI::Windowing::AppWindow const &window) noexcept;


  winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeIsland m_island { nullptr };
  winrt::event_token m_appWindowChangedToken;
  winrt::Microsoft::ReactNative::ReactNativeIsland::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::Microsoft::ReactNative::ContentSizePolicy m_resizePolicy{winrt::Microsoft::ReactNative::ContentSizePolicy::ResizeContentToParentWindow};
  winrt::Microsoft::UI::Content::IContentSiteBridge m_bridge{nullptr};
  winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{nullptr};
  winrt::Microsoft::ReactNative::LayoutConstraints m_layoutConstraints{{0, 0}, {0, 0}, winrt::Microsoft::ReactNative::LayoutDirection::Undefined};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactNativeWindow : ReactNativeWindowT<ReactNativeWindow, implementation::ReactNativeWindow> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
