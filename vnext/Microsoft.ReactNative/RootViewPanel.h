// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "RootViewPanel.g.h"

#include "Views/KeyboardEventHandler.h"
#include "Views/ViewPanel.h"

namespace Microsoft::ReactNative {
class TouchEventHandler;
class PreviewKeyboardEventHandlerOnRoot;
} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

struct RootViewPanel : RootViewPanelT<RootViewPanel, ViewPanel> {
  RootViewPanel(winrt::Microsoft::ReactNative::IReactContext reactContext) noexcept;
  ~RootViewPanel() noexcept;

 private:
  std::shared_ptr<::Microsoft::ReactNative::TouchEventHandler> m_touchEventHandler;
  std::shared_ptr<::Microsoft::ReactNative::PreviewKeyboardEventHandlerOnRoot> m_previewKeyboardEventHandlerOnRoot;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct RootViewPanel : RootViewPanelT<RootViewPanel, implementation::RootViewPanel> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
