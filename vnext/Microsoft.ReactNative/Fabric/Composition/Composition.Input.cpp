// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.Input.h"

namespace winrt::Microsoft::ReactNative::Composition::Input::implementation {

KeyRoutedEventArgs::KeyRoutedEventArgs(facebook::react::Tag tag, uint32_t msg, uint64_t wParam, int64_t lParam)
    : m_tag(tag)
{
    bool fUp = (msg == WM_KEYUP || msg == WM_SYSKEYUP);
    bool fSysKey = (msg == WM_SYSKEYUP || msg == WM_SYSKEYDOWN);

    if (fUp) {
        m_keyStatus.RepeatCount = 1;                             // bits 0-15, always 1
        m_keyStatus.ScanCode = (lParam & 0x00FF0000) >> 16;      // bits 16-23
        m_keyStatus.IsExtendedKey = (lParam & 0x01000000) >> 24; // bit 24
        m_keyStatus.IsMenuKeyDown = fSysKey ? ((lParam & 0x20000000) >> 29) : 0; // bit 29 for WM_SYSKEYUP, otherwise 0
        m_keyStatus.WasKeyDown = 1;                              // always 1 for WM_KEYUP
        m_keyStatus.IsKeyReleased = 1;                           // always 1 for WM_KEYUP
    } else {
        m_keyStatus.RepeatCount = (lParam & 0x0000FFFF);          // bits 0-15
        m_keyStatus.ScanCode = (lParam & 0x00FF0000) >> 16;       // bits 16-23
        m_keyStatus.IsExtendedKey = (lParam & 0x01000000) >> 24;  // bit 24
        m_keyStatus.IsMenuKeyDown = fSysKey ? ((lParam & 0x20000000) >> 29) : 0; // bit 29 for WM_SYSKEYDOWN, otherwise 0
        m_keyStatus.WasKeyDown = (lParam & 0x40000000) >> 30;     // bit 30
        m_keyStatus.IsKeyReleased = 0;                            // always 0 for WM_KEYDOWN
    }

    m_key = static_cast<winrt::Windows::System::VirtualKey>(wParam);
}

int32_t KeyRoutedEventArgs::OriginalSource() noexcept {
  return m_tag;
}

winrt::hstring KeyRoutedEventArgs::DeviceId() noexcept {
  return {};
}

bool KeyRoutedEventArgs::Handled() noexcept {
    return m_handled;
}

void KeyRoutedEventArgs::Handled(bool value) noexcept {
    m_handled = value;
}

winrt::Windows::System::VirtualKey KeyRoutedEventArgs::Key() noexcept {
    return m_key;
}

winrt::Windows::UI::Core::CorePhysicalKeyStatus KeyRoutedEventArgs::KeyStatus() noexcept {
    return m_keyStatus;
}

winrt::Windows::System::VirtualKey KeyRoutedEventArgs::OriginalKey() noexcept {
    return m_key;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
