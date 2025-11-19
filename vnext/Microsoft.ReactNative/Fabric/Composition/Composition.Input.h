// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.Input.Pointer.g.h"
#include "Composition.Input.PointerPoint.g.h"
#include "Composition.Input.PointerPointProperties.g.h"
#include "Composition.Input.PointerRoutedEventArgs.g.h"
#include <ReactContext.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Microsoft.UI.Input.h>

namespace winrt::Microsoft::ReactNative::Composition::Input::implementation {

struct KeyRoutedEventArgs : winrt::implements<
                                KeyRoutedEventArgs,
                                winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs,
                                winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  KeyRoutedEventArgs(
      facebook::react::Tag tag,
      uint32_t msg,
      uint64_t wParam,
      int64_t lParam,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source);
  KeyRoutedEventArgs(
      facebook::react::Tag tag,
      winrt::Microsoft::UI::Input::KeyEventArgs const &args,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source);

  int32_t OriginalSource() noexcept;
  winrt::hstring DeviceId() noexcept;
  bool Handled() noexcept;
  void Handled(bool value) noexcept;
  winrt::Windows::System::VirtualKey Key() noexcept;
  winrt::Microsoft::UI::Input::PhysicalKeyStatus KeyStatus() noexcept;
  winrt::Windows::System::VirtualKey OriginalKey() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource KeyboardSource() const noexcept;

 private:
  facebook::react::Tag m_tag{-1};
  bool m_handled{false};
  winrt::Windows::System::VirtualKey m_key;
  winrt::Microsoft::UI::Input::PhysicalKeyStatus m_keyStatus;
  const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource m_source;
};

struct CharacterReceivedRoutedEventArgs
    : winrt::implements<
          CharacterReceivedRoutedEventArgs,
          winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs,
          winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  CharacterReceivedRoutedEventArgs(
      facebook::react::Tag tag,
      uint32_t msg,
      uint64_t wParam,
      int64_t lParam,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source);
  CharacterReceivedRoutedEventArgs(
      facebook::react::Tag tag,
      winrt::Microsoft::UI::Input::CharacterReceivedEventArgs const &args,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source);

  int32_t OriginalSource() noexcept;
  bool Handled() noexcept;
  void Handled(bool value) noexcept;
  int32_t KeyCode() noexcept;
  winrt::Microsoft::UI::Input::PhysicalKeyStatus KeyStatus() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource KeyboardSource() const noexcept;

 private:
  facebook::react::Tag m_tag{-1};
  bool m_handled{false};
  int32_t m_keycode;
  winrt::Microsoft::UI::Input::PhysicalKeyStatus m_keyStatus;
  const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource m_source;
};

struct Pointer : PointerT<Pointer> {
  Pointer(winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType type, uint32_t id);

  winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType PointerDeviceType() const noexcept;
  uint32_t PointerId() const noexcept;

 private:
  winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType m_type;
  uint32_t m_id;
};

struct PointerPointProperties : PointerPointPropertiesT<PointerPointProperties> {
  PointerPointProperties(const winrt::Microsoft::UI::Input::PointerPointProperties &ppp);

  PointerPointProperties(
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
      PointerUpdateKind pointerUpdateKind,
      float pressure,
      float twist,
      float xTilt,
      float yTilt);

  winrt::Windows::Foundation::Rect ContactRect() noexcept;
  bool IsBarrelButtonPressed() noexcept;
  bool IsCanceled() noexcept;
  bool IsEraser() noexcept;
  bool IsHorizontalMouseWheel() noexcept;
  bool IsInRange() noexcept;
  bool IsInverted() noexcept;
  bool IsLeftButtonPressed() noexcept;
  bool IsMiddleButtonPressed() noexcept;
  bool IsPrimary() noexcept;
  bool IsRightButtonPressed() noexcept;
  bool IsXButton1Pressed() noexcept;
  bool IsXButton2Pressed() noexcept;
  int32_t MouseWheelDelta() noexcept;
  float Orientation() noexcept;
  PointerUpdateKind PointerUpdateKind() noexcept;
  float Pressure() noexcept;
  bool TouchConfidence() noexcept;
  float Twist() noexcept;
  float XTilt() noexcept;
  float YTilt() noexcept;

 private:
  winrt::Microsoft::UI::Input::PointerPointProperties m_sysPointerPointProps{nullptr};

  // When not using m_sysPointerPointProps
  bool m_isBarrelButtonPressed : 1;
  bool m_isCanceled : 1;
  bool m_isEraser : 1;
  bool m_isHorizontalMouseWheel : 1;
  bool m_isInRange : 1;
  bool m_isInverted : 1;
  bool m_isLeftButtonPressed : 1;
  bool m_isMiddleButtonPressed : 1;
  bool m_isPrimary : 1;
  bool m_isRightButtonPressed : 1;
  bool m_isXButton1Pressed : 1;
  bool m_isXButton2Pressed : 1;
  bool m_touchConfidence : 1;
  int32_t m_mouseWheelDelta;
  float m_orientation;
  winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind m_pointerUpdateKind;
  float m_pressure;
  float m_twist;
  float m_xTilt;
  float m_yTilt;
};

struct PointerPoint : PointerPointT<PointerPoint> {
  PointerPoint(const winrt::Microsoft::UI::Input::PointerPoint &pp, float scaleFactor);
  PointerPoint(
      const winrt::Microsoft::UI::Input::PointerPoint &pp,
      float scaleFactor,
      const winrt::Windows::Foundation::Point &offset);
  PointerPoint(HWND hwnd, uint32_t msg, uint64_t wParam, int64_t lParam, float scaleFactor);
  PointerPoint(
      HWND hwnd,
      uint32_t msg,
      uint64_t wParam,
      int64_t lParam,
      float scaleFactor,
      const winrt::Windows::Foundation::Point &offset);

  uint32_t FrameId() const noexcept;
  bool IsInContact() const noexcept;
  PointerDeviceType PointerDeviceType() const noexcept;
  uint32_t PointerId() const noexcept;
  winrt::Windows::Foundation::Point Position() const noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPointProperties Properties() const noexcept;
  uint64_t Timestamp() const noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint GetOffsetPoint(
      const winrt::Windows::Foundation::Point &offset) const noexcept;
  winrt::Microsoft::UI::Input::PointerPoint Inner() const noexcept;

 private:
  bool IsPointerMessage(uint32_t message) const noexcept;

  // Windows::Input
  winrt::Microsoft::UI::Input::PointerPoint m_sysPointerPoint{nullptr};
  // Non Windows::Input
  HWND m_hwnd;
  // WM_POINTER*
  POINTER_INFO m_pi = {0};
  // WM_*MOUSE
  uint32_t m_msg;
  uint64_t m_wParam;
  int64_t m_lParam;
  float m_scaleFactor;

  winrt::Windows::Foundation::Point m_offset;
};

struct PointerRoutedEventArgs : PointerRoutedEventArgsT<PointerRoutedEventArgs> {
  PointerRoutedEventArgs(
      const winrt::Microsoft::ReactNative::ReactContext &context,
      facebook::react::Tag tag,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pp,
      const winrt::Windows::System::VirtualKeyModifiers &virtualKeyModifiers);

  int32_t OriginalSource() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint GetCurrentPoint(int32_t tag) noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::Pointer Pointer() const noexcept;
  bool Handled() noexcept;
  void Handled(bool value) noexcept;
  winrt::Windows::System::VirtualKeyModifiers KeyModifiers() noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
  facebook::react::Tag m_tag{-1};
  bool m_handled{false};
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint m_pointerPoint{nullptr};
  winrt::Windows::System::VirtualKeyModifiers m_virtualKeyModifiers;
};

} // namespace winrt::Microsoft::ReactNative::Composition::Input::implementation
