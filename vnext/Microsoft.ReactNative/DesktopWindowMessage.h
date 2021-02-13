// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "DesktopWindowMessage.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct DesktopWindowMessage : DesktopWindowMessageT<DesktopWindowMessage> {
 public:
  DesktopWindowMessage(uint32_t uMsg, uint64_t wParam, int64_t lParam)
      : m_uMsg(uMsg), m_wParam(wParam), m_lParam(lParam) {}
  uint32_t uMsg() {
    return m_uMsg;
  }
  uint64_t wParam() {
    return m_wParam;
  }
  int64_t lParam() {
    return m_lParam;
  }

 private:
  uint32_t m_uMsg{};
  uint64_t m_wParam{};
  int64_t m_lParam{};
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct DesktopWindowMessage : DesktopWindowMessageT<DesktopWindowMessage, implementation::DesktopWindowMessage> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
