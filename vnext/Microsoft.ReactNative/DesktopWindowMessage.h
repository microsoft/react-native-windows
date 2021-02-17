// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "DesktopWindowMessage.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct DesktopWindowMessage : DesktopWindowMessageT<DesktopWindowMessage> {
 public:
  DesktopWindowMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept
      : m_msg(msg), m_wParam(wParam), m_lParam(lParam) {}
  uint32_t Msg() const noexcept {
    return m_msg;
  }
  uint64_t WParam() const noexcept {
    return m_wParam;
  }
  int64_t LParam() const noexcept {
    return m_lParam;
  }

 private:
  uint32_t m_msg{};
  uint64_t m_wParam{};
  int64_t m_lParam{};
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct DesktopWindowMessage : DesktopWindowMessageT<DesktopWindowMessage, implementation::DesktopWindowMessage> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
