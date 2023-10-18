// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.Input.h"
#include <windowsx.h>
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

PointerPointProperties::PointerPointProperties(const winrt::Windows::UI::Input::PointerPointProperties &ppp)
    : m_sysPointerPointProps(ppp) {}

PointerPointProperties::PointerPointProperties(
    bool isBarrelButtonPressed,
    bool isCanceled,
    bool isEraser,
    bool isHorizontalMouseWheel,
    bool isInRange,
    bool isInverted,
    bool isLeftButtonPressed,
    bool isMiddleButtonPressed,
    bool isPrimary,
    bool isRightButtonPressed,
    bool isXButton1Pressed,
    bool isXButton2Pressed,
    bool touchConfidence,
    int32_t mouseWheelDelta,
    float orientation,
    winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind pointerUpdateKind,
    float pressure,
    float twist,
    float xTilt,
    float yTilt)
    : m_isBarrelButtonPressed(isBarrelButtonPressed),
      m_isCanceled(isCanceled),
      m_isEraser(isEraser),
      m_isHorizontalMouseWheel(isHorizontalMouseWheel),
      m_isInRange(isInRange),
      m_isInverted(isInverted),
      m_isLeftButtonPressed(isLeftButtonPressed),
      m_isMiddleButtonPressed(isMiddleButtonPressed),
      m_isPrimary(isPrimary),
      m_isRightButtonPressed(isRightButtonPressed),
      m_isXButton1Pressed(isXButton1Pressed),
      m_isXButton2Pressed(isXButton2Pressed),
      m_touchConfidence(touchConfidence),
      m_mouseWheelDelta(mouseWheelDelta),
      m_orientation(orientation),
      m_pointerUpdateKind(pointerUpdateKind),
      m_pressure(pressure),
      m_twist(twist),
      m_xTilt(xTilt),
      m_yTilt(yTilt) {}

winrt::Windows::Foundation::Rect PointerPointProperties::ContactRect() noexcept {
  return m_sysPointerPointProps.ContactRect();
}

bool PointerPointProperties::IsBarrelButtonPressed() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsBarrelButtonPressed() : m_isBarrelButtonPressed;
}

bool PointerPointProperties::IsCanceled() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsCanceled() : m_isCanceled;
}

bool PointerPointProperties::IsEraser() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsEraser() : m_isEraser;
}

bool PointerPointProperties::IsHorizontalMouseWheel() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsHorizontalMouseWheel() : m_isHorizontalMouseWheel;
}

bool PointerPointProperties::IsInRange() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsInRange() : m_isInRange;
}

bool PointerPointProperties::IsInverted() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsInverted() : m_isInverted;
}

bool PointerPointProperties::IsLeftButtonPressed() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsLeftButtonPressed() : m_isLeftButtonPressed;
}

bool PointerPointProperties::IsMiddleButtonPressed() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsMiddleButtonPressed() : m_isMiddleButtonPressed;
}

bool PointerPointProperties::IsPrimary() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsPrimary() : m_isPrimary;
}

bool PointerPointProperties::IsRightButtonPressed() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsRightButtonPressed() : m_isRightButtonPressed;
}

bool PointerPointProperties::IsXButton1Pressed() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsXButton1Pressed() : m_isXButton1Pressed;
}

bool PointerPointProperties::IsXButton2Pressed() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.IsXButton2Pressed() : m_isXButton2Pressed;
}

int32_t PointerPointProperties::MouseWheelDelta() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.MouseWheelDelta() : m_mouseWheelDelta;
}

float PointerPointProperties::Orientation() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.Orientation() : m_orientation;
}

PointerUpdateKind PointerPointProperties::PointerUpdateKind() noexcept {
  return m_sysPointerPointProps ? static_cast<winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind>(
                                      m_sysPointerPointProps.PointerUpdateKind())
                                : m_pointerUpdateKind;
}

float PointerPointProperties::Pressure() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.Pressure() : m_pressure;
}

bool PointerPointProperties::TouchConfidence() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.TouchConfidence() : m_touchConfidence;
}

float PointerPointProperties::Twist() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.Twist() : m_twist;
}

float PointerPointProperties::XTilt() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.XTilt() : m_xTilt;
}

float PointerPointProperties::YTilt() noexcept {
  return m_sysPointerPointProps ? m_sysPointerPointProps.YTilt() : m_yTilt;
}

PointerPoint::PointerPoint(const winrt::Windows::UI::Input::PointerPoint &pp) : m_sysPointerPoint(pp) {}

PointerPoint::PointerPoint(uint32_t msg, uint64_t wParam, int64_t lParam) {
  m_msg = msg;
  m_wParam = wParam;
  m_lParam = lParam;
  if (IsPointerMessage(msg)) {
    const unsigned int pointerId = GET_POINTERID_WPARAM(wParam);
    bool result = ::GetPointerInfo(pointerId, &m_pi);
    assert(result);
  }
}

uint32_t PointerPoint::FrameId() noexcept {
  return m_sysPointerPoint ? m_sysPointerPoint.FrameId() : m_pi.frameId;
}

bool PointerPoint::IsInContact() noexcept {
  return m_sysPointerPoint ? m_sysPointerPoint.IsInContact()
                           : ((m_pi.pointerFlags & POINTER_FLAG_INCONTACT) == POINTER_FLAG_INCONTACT);
}

winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType PointerPoint::PointerDeviceType() noexcept {
  if (m_sysPointerPoint) {
    return static_cast<winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType>(
        m_sysPointerPoint.PointerDevice().PointerDeviceType());
  }

  if (m_pi.pointerId) {
    switch (m_pi.pointerType) {
      case PT_MOUSE:
        return winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse;
      case PT_PEN:
        return winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Pen;
      case PT_TOUCHPAD:
        return winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Touchpad;
      case PT_TOUCH:
      case PT_POINTER:
      default:
        return winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Touchpad;
    }
  }

  return winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse;
}

uint32_t PointerPoint::PointerId() noexcept {
  return m_sysPointerPoint ? m_sysPointerPoint.PointerId() : (m_pi.pointerId ? m_pi.pointerId : 1 /* MOUSE */);
}

winrt::Windows::Foundation::Point PointerPoint::Position() noexcept {
  return m_sysPointerPoint
      ? m_sysPointerPoint.Position()
      : (m_pi.pointerId
             ? winrt::Windows::Foundation::
                   Point{static_cast<float>(m_pi.ptPixelLocation.x), static_cast<float>(m_pi.ptPixelLocation.y)}
             : winrt::Windows::Foundation::Point{
                   static_cast<float>(GET_X_LPARAM(m_lParam)), static_cast<float>(GET_Y_LPARAM(m_lParam))});
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPointProperties PointerPoint::Properties() noexcept {
  if (m_sysPointerPoint) {
    return winrt::make<PointerPointProperties>(m_sysPointerPoint.Properties());
  }

  if (m_pi.pointerId) {
    auto pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::Other;
    switch (m_pi.ButtonChangeType) {
      case POINTER_CHANGE_FIRSTBUTTON_DOWN:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::LeftButtonPressed;
        break;
      case POINTER_CHANGE_FIRSTBUTTON_UP:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::LeftButtonReleased;
        break;
      case POINTER_CHANGE_SECONDBUTTON_DOWN:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonPressed;
        break;
      case POINTER_CHANGE_SECONDBUTTON_UP:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonReleased;
        break;
      case POINTER_CHANGE_THIRDBUTTON_DOWN:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::MiddleButtonPressed;
        break;
      case POINTER_CHANGE_THIRDBUTTON_UP:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::MiddleButtonReleased;
        break;
      case POINTER_CHANGE_FOURTHBUTTON_DOWN:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton1Pressed;
        break;
      case POINTER_CHANGE_FOURTHBUTTON_UP:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton1Released;
        break;
      case POINTER_CHANGE_FIFTHBUTTON_DOWN:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton2Pressed;
        break;
      case POINTER_CHANGE_FIFTHBUTTON_UP:
        pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton2Released;
        break;
    }

    return winrt::make<PointerPointProperties>(
        false, // TODO isBarrelButtonPressed,
        (m_pi.pointerFlags & POINTER_FLAG_CANCELED) == POINTER_FLAG_CANCELED, // isCanceled
        false, // TODO isEraser,
        (m_pi.pointerFlags & POINTER_FLAG_HWHEEL) == POINTER_FLAG_HWHEEL, // isHorizontalMouseWheel
        (m_pi.pointerFlags & POINTER_FLAG_INRANGE) == POINTER_FLAG_INRANGE, // isInRange
        false, // TODO isInverted,
        (m_pi.pointerFlags & POINTER_FLAG_FIRSTBUTTON) == POINTER_FLAG_FIRSTBUTTON, // isLeftButtonPressed
        (m_pi.pointerFlags & POINTER_FLAG_THIRDBUTTON) == POINTER_FLAG_THIRDBUTTON, // isMiddleButtonPressed
        (m_pi.pointerFlags & POINTER_FLAG_PRIMARY) == POINTER_FLAG_PRIMARY, // isPrimary
        (m_pi.pointerFlags & POINTER_FLAG_SECONDBUTTON) == POINTER_FLAG_SECONDBUTTON, // isRightButtonPressed
        (m_pi.pointerFlags & POINTER_FLAG_FOURTHBUTTON) == POINTER_FLAG_FOURTHBUTTON, // isXButton1Pressed
        (m_pi.pointerFlags & POINTER_FLAG_FIFTHBUTTON) == POINTER_FLAG_FIFTHBUTTON, // isXButton2Pressed
        (m_pi.pointerFlags & POINTER_FLAG_CONFIDENCE) == POINTER_FLAG_CONFIDENCE, // touchConfidence
        0, // mouseWheelDelta would come through a WM_(H)MOUSEWHEEL message, which wont have a POINTER_INFO struct,
        0.0f, // TODO orientation,
        pointerUpdateKind,
        0.0f, // TODO pressure,
        0.0f, // TODO twist,
        0.0f, // TODO xTilt,
        0.0f // TODO yTilt
    );
  }

  auto pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::Other;
  switch (m_msg) {
    case WM_LBUTTONDOWN:
      pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::LeftButtonPressed;
      break;
    case WM_LBUTTONUP:
      pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::LeftButtonReleased;
      break;
    case WM_RBUTTONDOWN:
      pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonPressed;
      break;
    case WM_RBUTTONUP:
      pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonReleased;
      break;
    case WM_MBUTTONDOWN:
      pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::MiddleButtonPressed;
      break;
    case WM_MBUTTONUP:
      pointerUpdateKind = winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::MiddleButtonReleased;
      break;
    case WM_XBUTTONDOWN:
      pointerUpdateKind = (GET_XBUTTON_WPARAM(m_wParam) == XBUTTON1)
          ? winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton1Pressed
          : winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton2Pressed;
      break;
    case WM_XBUTTONUP:
      pointerUpdateKind = (GET_XBUTTON_WPARAM(m_wParam) == XBUTTON1)
          ? winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton1Released
          : winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton2Released;
      break;
  }

  return winrt::make<PointerPointProperties>(
      false, // isBarrelButtonPressed,
      false, // isCanceled,
      false, // isEraser,
      m_msg == WM_MOUSEHWHEEL, // isHorizontalMouseWheel,
      false, // isInRange,
      false, // isInverted,
      (m_wParam & MK_LBUTTON) == MK_LBUTTON,
      (m_wParam & MK_MBUTTON) == MK_MBUTTON,
      true, // isPrimary,
      (m_wParam & MK_RBUTTON) == MK_RBUTTON,
      (m_wParam & MK_XBUTTON1) == MK_XBUTTON1,
      (m_wParam & MK_XBUTTON2) == MK_XBUTTON2,
      true, // touchConfidence,
      (m_msg == WM_MOUSEWHEEL || m_msg == WM_MOUSEHWHEEL) ? GET_WHEEL_DELTA_WPARAM(m_wParam) : 0, // mouseWheelDelta,
      0.0f, // orientation,
      pointerUpdateKind,
      0.0f, // pressure,
      0.0f, // twist,
      0.0f, // xTilt,
      0.0f // yTilt
  );
}

uint64_t PointerPoint::Timestamp() noexcept {
  return m_sysPointerPoint ? m_sysPointerPoint.Timestamp() : m_pi.dwTime;
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPoint PointerPoint::GetTransformedPoint(
    const IPointerPointTransform &transform) noexcept {
  return winrt::make<PointerPoint>(m_sysPointerPoint);
}

bool PointerPoint::IsPointerMessage(uint32_t message) noexcept {
  constexpr uint32_t WM_POINTERFIRST = 0x0245;
  constexpr uint32_t WM_POINTERLAST = 0x0257;
  return (message >= WM_POINTERFIRST && message <= WM_POINTERLAST);
}

PointerRoutedEventArgs::PointerRoutedEventArgs(
    facebook::react::Tag tag,
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pp,
    const winrt::Windows::System::VirtualKeyModifiers &virtualKeyModifiers)
    : m_tag(tag), m_virtualKeyModifiers(virtualKeyModifiers) {
  m_pointerPoint = pp;
}

int32_t PointerRoutedEventArgs::OriginalSource() noexcept {
  return m_tag;
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPoint PointerRoutedEventArgs::GetCurrentPoint(
    int32_t tag) noexcept {
  assert(tag == -1); // TODO: Non-null tags need to apply a transform
  return m_pointerPoint;
}

bool PointerRoutedEventArgs::Handled() noexcept {
  return m_handled;
}

void PointerRoutedEventArgs::Handled(bool value) noexcept {
  m_handled = value;
}

winrt::Windows::System::VirtualKeyModifiers PointerRoutedEventArgs::KeyModifiers() noexcept {
  return m_virtualKeyModifiers;
}

} // namespace winrt::Microsoft::ReactNative::Composition::Input::implementation
