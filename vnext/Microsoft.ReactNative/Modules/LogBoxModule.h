// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "../../codegen/NativeLogBoxSpec.g.h"
#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

REACT_MODULE(LogBox)
struct LogBox : public std::enable_shared_from_this<LogBox> {
  using ModuleSpec = ReactNativeSpecs::LogBoxSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(Show, L"show") void Show() noexcept;
  REACT_METHOD(Hide, L"hide") void Hide() noexcept;

 private:
  static void RegisterWndClass() noexcept;

  void ShowOnUIThread() noexcept;
  void HideOnUIThread() noexcept;

  React::ReactContext m_context;
  HWND m_hwnd{nullptr};
};

} // namespace Microsoft::ReactNative
