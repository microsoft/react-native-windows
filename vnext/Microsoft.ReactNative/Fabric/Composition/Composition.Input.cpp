// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.Input.h"
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.UI.Input.h>

namespace winrt::Microsoft::ReactNative::Composition::Input::implementation {

KeyRoutedEventArgs::KeyRoutedEventArgs(facebook::react::Tag tag, uint32_t msg, uint64_t wParam, int64_t lParam)
    : m_tag(tag) {
  bool fUp = (msg == WM_KEYUP || msg == WM_SYSKEYUP);
  bool fSysKey = (msg == WM_SYSKEYUP || msg == WM_SYSKEYDOWN);

  if (fUp) {
    m_keyStatus.RepeatCount = 1; // bits 0-15, always 1
    m_keyStatus.ScanCode = (lParam & 0x00FF0000) >> 16; // bits 16-23
    m_keyStatus.IsExtendedKey = (lParam & 0x01000000) >> 24; // bit 24
    m_keyStatus.IsMenuKeyDown = fSysKey ? ((lParam & 0x20000000) >> 29) : 0; // bit 29 for WM_SYSKEYUP, otherwise 0
    m_keyStatus.WasKeyDown = 1; // always 1 for WM_KEYUP
    m_keyStatus.IsKeyReleased = 1; // always 1 for WM_KEYUP
  } else {
    m_keyStatus.RepeatCount = (lParam & 0x0000FFFF); // bits 0-15
    m_keyStatus.ScanCode = (lParam & 0x00FF0000) >> 16; // bits 16-23
    m_keyStatus.IsExtendedKey = (lParam & 0x01000000) >> 24; // bit 24
    m_keyStatus.IsMenuKeyDown = fSysKey ? ((lParam & 0x20000000) >> 29) : 0; // bit 29 for WM_SYSKEYDOWN, otherwise 0
    m_keyStatus.WasKeyDown = (lParam & 0x40000000) >> 30; // bit 30
    m_keyStatus.IsKeyReleased = 0; // always 0 for WM_KEYDOWN
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

PointerPointProperties::PointerPointProperties(const winrt::Windows::UI::Input::PointerPointProperties& ppp)
: m_systemPPP(ppp) {}

winrt::Windows::Foundation::Rect PointerPointProperties::ContactRect() noexcept{
  return m_systemPPP.ContactRect();
}

bool PointerPointProperties::IsBarrelButtonPressed() noexcept {
  return m_systemPPP.IsBarrelButtonPressed();
}

bool PointerPointProperties::IsCanceled() noexcept {
  return m_systemPPP.IsCanceled();
}

bool PointerPointProperties::IsEraser() noexcept {
  return m_systemPPP.IsEraser();
}

bool PointerPointProperties::IsHorizontalMouseWheel() noexcept {
  return m_systemPPP.IsHorizontalMouseWheel();
}

bool PointerPointProperties::IsInRange() noexcept {
  return m_systemPPP.IsInRange();
}

bool PointerPointProperties::IsInverted() noexcept {
  return m_systemPPP.IsInverted();
}

bool PointerPointProperties::IsLeftButtonPressed() noexcept {
  return m_systemPPP.IsLeftButtonPressed();
}

bool PointerPointProperties::IsMiddleButtonPressed() noexcept {
  return m_systemPPP.IsMiddleButtonPressed();
}

bool PointerPointProperties::IsPrimary() noexcept {
  return m_systemPPP.IsPrimary();
}

bool PointerPointProperties::IsRightButtonPressed() noexcept {
  return m_systemPPP.IsRightButtonPressed();
}

bool PointerPointProperties::IsXButton1Pressed() noexcept {
  return m_systemPPP.IsXButton1Pressed();
}

bool PointerPointProperties::IsXButton2Pressed() noexcept {
  return m_systemPPP.IsXButton2Pressed();
}

int32_t PointerPointProperties::MouseWheelDelta() noexcept {
  return m_systemPPP.MouseWheelDelta();
}

float PointerPointProperties::Orientation() noexcept {
  return m_systemPPP.Orientation();
}

PointerUpdateKind PointerPointProperties::PointerUpdateKind() noexcept {
  return static_cast<winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind>(m_systemPPP.PointerUpdateKind());
}

float PointerPointProperties::Pressure() noexcept {
  return m_systemPPP.Pressure();
}

bool PointerPointProperties::TouchConfidence() noexcept {
  return m_systemPPP.TouchConfidence();
}

float PointerPointProperties::Twist() noexcept {
  return m_systemPPP.Twist();
}

float PointerPointProperties::XTilt() noexcept {
  return m_systemPPP.XTilt();
}

float PointerPointProperties::YTilt() noexcept {
  return m_systemPPP.YTilt();
}

PointerPoint::PointerPoint(const winrt::Windows::UI::Input::PointerPoint& pp) : m_systemPP(pp) {}

uint32_t PointerPoint::FrameId() noexcept {
  return m_systemPP.FrameId();
}

bool PointerPoint::IsInContact() noexcept {
  return m_systemPP.IsInContact();
}

winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType PointerPoint::PointerDeviceType() noexcept {
  return static_cast<winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType>(m_systemPP.PointerDevice().PointerDeviceType());
}

uint32_t PointerPoint::PointerId() noexcept {
  return m_systemPP.PointerId();
}

winrt::Windows::Foundation::Point PointerPoint::Position() noexcept {
  return m_systemPP.Position();
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPointProperties PointerPoint::Properties() noexcept {
  return winrt::make<PointerPointProperties>(m_systemPP.Properties());
}

uint64_t PointerPoint::Timestamp() noexcept {
  return m_systemPP.Timestamp();
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPoint PointerPoint::GetTransformedPoint(
    const IPointerPointTransform& transform) noexcept {
    //return winrt::make< PointerPoint>(m_systemPP.GetTransformedPoint(transform)); // TODO pass transform on
  return winrt::make<PointerPoint>(m_systemPP);
}

PointerRoutedEventArgs::PointerRoutedEventArgs(facebook::react::Tag tag, const winrt::Windows::UI::Input::PointerPoint& pp) : m_tag(tag) {
  m_pp = winrt::make<PointerPoint>(pp); // TODO transform?
}

int32_t PointerRoutedEventArgs::OriginalSource() noexcept {
  return m_tag;
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPoint PointerRoutedEventArgs::GetCurrentPoint(int32_t tag) noexcept {
  return m_pp; // Apply transform?
}

bool PointerRoutedEventArgs::Handled() noexcept {
  return m_handled;
}

void PointerRoutedEventArgs::Handled(bool value) noexcept {
  m_handled = value;
}

winrt::Windows::System::VirtualKeyModifiers PointerRoutedEventArgs::KeyModifiers() noexcept {
  assert(false);
  return winrt::Windows::System::VirtualKeyModifiers::None;
}

} // namespace winrt::Microsoft::ReactNative::Composition::Input::implementation
