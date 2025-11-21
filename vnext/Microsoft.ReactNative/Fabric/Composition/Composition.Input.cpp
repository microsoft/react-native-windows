// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.Input.h"
#include <Fabric/FabricUIManagerModule.h>
#include <windowsx.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.UI.Input.h>

namespace winrt::Microsoft::ReactNative::Composition::Input::implementation {

KeyRoutedEventArgs::KeyRoutedEventArgs(
    facebook::react::Tag tag,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source)
    : m_tag(tag), m_source(source) {
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

KeyRoutedEventArgs::KeyRoutedEventArgs(
    facebook::react::Tag tag,
    winrt::Microsoft::UI::Input::KeyEventArgs const &args,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source)
    : m_tag(tag), m_source(source) {
  auto keyStatus = args.KeyStatus();
  m_keyStatus.RepeatCount = keyStatus.RepeatCount;
  m_keyStatus.ScanCode = keyStatus.ScanCode;
  m_keyStatus.IsExtendedKey = keyStatus.IsExtendedKey;
  m_keyStatus.IsMenuKeyDown = keyStatus.IsMenuKeyDown;
  m_keyStatus.WasKeyDown = keyStatus.WasKeyDown;
  m_keyStatus.IsKeyReleased = keyStatus.IsKeyReleased;
  m_key = args.VirtualKey();
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

winrt::Microsoft::UI::Input::PhysicalKeyStatus KeyRoutedEventArgs::KeyStatus() noexcept {
  return m_keyStatus;
}

winrt::Windows::System::VirtualKey KeyRoutedEventArgs::OriginalKey() noexcept {
  return m_key;
}
winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource KeyRoutedEventArgs::KeyboardSource() const noexcept {
  return m_source;
}

CharacterReceivedRoutedEventArgs::CharacterReceivedRoutedEventArgs(
    facebook::react::Tag tag,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source)
    : m_source(source) {
  m_keycode = static_cast<int32_t>(wParam);
  m_keyStatus.RepeatCount = (lParam & 0x0000FFFF); // bits 0-15
  m_keyStatus.ScanCode = (lParam & 0x00FF0000) >> 16; // bits 16-23
  m_keyStatus.IsExtendedKey = (lParam & 0x01000000) >> 24; // bit 24
  m_keyStatus.IsMenuKeyDown = (lParam & 0x20000000) >> 29; // bit 29
  m_keyStatus.WasKeyDown = (lParam & 0x40000000) >> 30; // bit 30
  m_keyStatus.IsKeyReleased = (lParam & 0x80000000) >> 31; // bit 31
}

CharacterReceivedRoutedEventArgs::CharacterReceivedRoutedEventArgs(
    facebook::react::Tag tag,
    winrt::Microsoft::UI::Input::CharacterReceivedEventArgs const &args,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source)
    : m_tag(tag), m_source(source) {
  auto keyStatus = args.KeyStatus();
  m_keyStatus.RepeatCount = keyStatus.RepeatCount;
  m_keyStatus.ScanCode = keyStatus.ScanCode;
  m_keyStatus.IsExtendedKey = keyStatus.IsExtendedKey;
  m_keyStatus.IsMenuKeyDown = keyStatus.IsMenuKeyDown;
  m_keyStatus.WasKeyDown = keyStatus.WasKeyDown;
  m_keyStatus.IsKeyReleased = keyStatus.IsKeyReleased;
  m_keycode = args.KeyCode();
}

int32_t CharacterReceivedRoutedEventArgs::OriginalSource() noexcept {
  return m_tag;
}
bool CharacterReceivedRoutedEventArgs::Handled() noexcept {
  return m_handled;
}
void CharacterReceivedRoutedEventArgs::Handled(bool value) noexcept {
  m_handled = value;
}
int32_t CharacterReceivedRoutedEventArgs::KeyCode() noexcept {
  return m_keycode;
}
winrt::Microsoft::UI::Input::PhysicalKeyStatus CharacterReceivedRoutedEventArgs::KeyStatus() noexcept {
  return m_keyStatus;
}
winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource CharacterReceivedRoutedEventArgs::KeyboardSource()
    const noexcept {
  return m_source;
}

Pointer::Pointer(winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType type, uint32_t id)
    : m_type(type), m_id(id) {}

winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType Pointer::PointerDeviceType() const noexcept {
  return m_type;
}

uint32_t Pointer::PointerId() const noexcept {
  return m_id;
}

PointerPointProperties::PointerPointProperties(const winrt::Microsoft::UI::Input::PointerPointProperties &ppp)
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
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsBarrelButtonPressed();
  }
  return m_isBarrelButtonPressed;
}

bool PointerPointProperties::IsCanceled() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsCanceled();
  }
  return m_isCanceled;
}

bool PointerPointProperties::IsEraser() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsEraser();
  }
  return m_isEraser;
}

bool PointerPointProperties::IsHorizontalMouseWheel() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsHorizontalMouseWheel();
  }
  return m_isHorizontalMouseWheel;
}

bool PointerPointProperties::IsInRange() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsInRange();
  }
  return m_isInRange;
}

bool PointerPointProperties::IsInverted() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsInverted();
  }
  return m_isInverted;
}

bool PointerPointProperties::IsLeftButtonPressed() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsLeftButtonPressed();
  }
  return m_isLeftButtonPressed;
}

bool PointerPointProperties::IsMiddleButtonPressed() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsMiddleButtonPressed();
  }
  return m_isMiddleButtonPressed;
}

bool PointerPointProperties::IsPrimary() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsPrimary();
  }
  return m_isPrimary;
}

bool PointerPointProperties::IsRightButtonPressed() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsRightButtonPressed();
  }
  return m_isRightButtonPressed;
}

bool PointerPointProperties::IsXButton1Pressed() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsXButton1Pressed();
  }
  return m_isXButton1Pressed;
}

bool PointerPointProperties::IsXButton2Pressed() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.IsXButton2Pressed();
  }
  return m_isXButton2Pressed;
}

int32_t PointerPointProperties::MouseWheelDelta() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.MouseWheelDelta();
  }
  return m_mouseWheelDelta;
}

float PointerPointProperties::Orientation() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.Orientation();
  }
  return m_orientation;
}

PointerUpdateKind PointerPointProperties::PointerUpdateKind() noexcept {
  if (m_sysPointerPointProps) {
    return static_cast<winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind>(
        m_sysPointerPointProps.PointerUpdateKind());
  }
  return m_pointerUpdateKind;
}

float PointerPointProperties::Pressure() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.Pressure();
  }
  return m_pressure;
}

bool PointerPointProperties::TouchConfidence() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.TouchConfidence();
  }
  return m_touchConfidence;
}

float PointerPointProperties::Twist() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.Twist();
  }
  return m_twist;
}

float PointerPointProperties::XTilt() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.XTilt();
  }
  return m_xTilt;
}

float PointerPointProperties::YTilt() noexcept {
  if (m_sysPointerPointProps) {
    return m_sysPointerPointProps.YTilt();
  }
  return m_yTilt;
}

PointerPoint::PointerPoint(const winrt::Microsoft::UI::Input::PointerPoint &pp, float scaleFactor)
    : m_sysPointerPoint(pp), m_scaleFactor(scaleFactor), m_offset({0, 0}) {}

PointerPoint::PointerPoint(
    const winrt::Microsoft::UI::Input::PointerPoint &pp,
    float scaleFactor,
    const winrt::Windows::Foundation::Point &offset)
    : m_sysPointerPoint(pp), m_scaleFactor(scaleFactor), m_offset(offset) {}

PointerPoint::PointerPoint(
    HWND hwnd,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam,
    float scaleFactor,
    const winrt::Windows::Foundation::Point &offset)
    : m_offset(offset), m_hwnd(hwnd), m_msg(msg), m_wParam(wParam), m_lParam(lParam), m_scaleFactor(scaleFactor) {
  if (IsPointerMessage(msg)) {
    const unsigned int pointerId = GET_POINTERID_WPARAM(wParam);
    bool result = ::GetPointerInfo(pointerId, &m_pi);
    assert(result);
  }
}

PointerPoint::PointerPoint(HWND hwnd, uint32_t msg, uint64_t wParam, int64_t lParam, float scaleFactor)
    : PointerPoint(hwnd, msg, wParam, lParam, scaleFactor, {0, 0}) {}

uint32_t PointerPoint::FrameId() const noexcept {
  if (m_sysPointerPoint) {
    return m_sysPointerPoint.FrameId();
  }
  return m_pi.frameId;
}

bool PointerPoint::IsInContact() const noexcept {
  if (m_sysPointerPoint) {
    return m_sysPointerPoint.IsInContact();
  }
  return ((m_pi.pointerFlags & POINTER_FLAG_INCONTACT) == POINTER_FLAG_INCONTACT);
}

winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType PointerPoint::PointerDeviceType() const noexcept {
  if (m_sysPointerPoint) {
    return static_cast<winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType>(
        m_sysPointerPoint.PointerDeviceType());
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

uint32_t PointerPoint::PointerId() const noexcept {
  if (m_sysPointerPoint) {
    return m_sysPointerPoint.PointerId();
  }
  return (m_pi.pointerId ? m_pi.pointerId : 1 /* MOUSE */);
}

winrt::Windows::Foundation::Point PointerPoint::Position() const noexcept {
  if (m_sysPointerPoint) {
    auto pos = m_sysPointerPoint.Position();
    return {pos.X - (m_offset.X / m_scaleFactor), pos.Y - (m_offset.Y / m_scaleFactor)};
  }

  assert(m_hwnd);
  POINT clientPoint{
      m_pi.pointerId ? m_pi.ptPixelLocation.x : GET_X_LPARAM(m_lParam),
      m_pi.pointerId ? m_pi.ptPixelLocation.y : GET_Y_LPARAM(m_lParam)};
  if (m_pi.pointerId || m_msg == WM_MOUSEWHEEL || m_msg == WM_MOUSEHWHEEL) {
    ScreenToClient(m_hwnd, &clientPoint);
  }
  return winrt::Windows::Foundation::Point{
      static_cast<float>(clientPoint.x / m_scaleFactor) - (m_offset.X / m_scaleFactor),
      static_cast<float>(clientPoint.y / m_scaleFactor) - (m_offset.Y / m_scaleFactor)};
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPointProperties PointerPoint::Properties() const noexcept {
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

uint64_t PointerPoint::Timestamp() const noexcept {
  if (m_sysPointerPoint) {
    return m_sysPointerPoint.Timestamp();
  }
  return m_pi.dwTime;
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPoint PointerPoint::GetOffsetPoint(
    const winrt::Windows::Foundation::Point &offset) const noexcept {
  if (m_sysPointerPoint) {
    return winrt::make<PointerPoint>(m_sysPointerPoint, m_scaleFactor, offset);
  }
  return winrt::make<PointerPoint>(m_hwnd, m_msg, m_wParam, m_lParam, m_scaleFactor, offset);
}

bool PointerPoint::IsPointerMessage(uint32_t message) const noexcept {
  constexpr uint32_t WM_POINTERFIRST = 0x0245;
  constexpr uint32_t WM_POINTERLAST = 0x0257;
  return (message >= WM_POINTERFIRST && message <= WM_POINTERLAST);
}

winrt::Microsoft::UI::Input::PointerPoint PointerPoint::Inner() const noexcept {
  return m_sysPointerPoint;
}

PointerRoutedEventArgs::PointerRoutedEventArgs(
    const winrt::Microsoft::ReactNative::ReactContext &context,
    facebook::react::Tag tag,
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pp,
    const winrt::Windows::System::VirtualKeyModifiers &virtualKeyModifiers)
    : m_context(context), m_tag(tag), m_virtualKeyModifiers(virtualKeyModifiers) {
  m_pointerPoint = pp;
}

int32_t PointerRoutedEventArgs::OriginalSource() noexcept {
  return m_tag;
}

winrt::Microsoft::ReactNative::Composition::Input::PointerPoint PointerRoutedEventArgs::GetCurrentPoint(
    int32_t tag) noexcept {
  if (tag == -1) {
    return m_pointerPoint;
  }

  auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties());

  const auto &viewRegistry = fabricuiManager->GetViewRegistry();
  auto targetComponentViewDescriptor = viewRegistry.componentViewDescriptorWithTag(tag);

  auto clientRect =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(targetComponentViewDescriptor.view)
          ->getClientRect();

  return m_pointerPoint.GetOffsetPoint({static_cast<float>(clientRect.left), static_cast<float>(clientRect.top)});
}

winrt::Microsoft::ReactNative::Composition::Input::Pointer PointerRoutedEventArgs::Pointer() const noexcept {
  return winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::Pointer>(
      m_pointerPoint.PointerDeviceType(), m_pointerPoint.PointerId());
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
