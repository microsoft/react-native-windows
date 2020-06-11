// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

REACT_MODULE(LogBox)
struct LogBox : public std::enable_shared_from_this<LogBox> {
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(show) void show() noexcept;
  REACT_METHOD(hide) void hide() noexcept;

 private:
  void showOnUIThread() noexcept;
  void hideOnUIThread() noexcept;
  void onPopupClosed() noexcept;

  React::ReactContext m_context;
  xaml::Controls::Primitives::Popup m_popup{nullptr};
  React::ReactRootView m_logBoxContent{nullptr};
  xaml::FrameworkElement::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::event_token m_tokenClosed;
};

} // namespace Microsoft::ReactNative
