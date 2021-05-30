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

  REACT_METHOD(Show, L"show") void Show() noexcept;
  REACT_METHOD(Hide, L"hide") void Hide() noexcept;

 private:
  void ShowOnUIThread() noexcept;
  void HideOnUIThread() noexcept;

  React::ReactContext m_context;
  xaml::Controls::Primitives::Popup m_popup{nullptr};
  React::ReactRootView m_logBoxContent{nullptr};
  xaml::FrameworkElement::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::event_token m_tokenClosed;
};

} // namespace Microsoft::ReactNative
