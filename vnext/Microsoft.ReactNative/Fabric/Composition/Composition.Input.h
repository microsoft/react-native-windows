// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.Input.PointerPoint.g.h"
#include "Composition.Input.PointerPointProperties.g.h"
#include "Composition.Input.PointerRoutedEventArgs.g.h"
#include <react/renderer/core/ReactPrimitives.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Input.h>
#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Input.h>
#endif

namespace winrt::Microsoft::ReactNative::Composition::Input::implementation {

struct KeyRoutedEventArgs : winrt::implements<
                                KeyRoutedEventArgs,
                                winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs,
                                winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  KeyRoutedEventArgs(facebook::react::Tag tag, uint32_t msg, uint64_t wParam, int64_t lParam);
#ifdef USE_WINUI3
  KeyRoutedEventArgs(facebook::react::Tag tag, winrt::Microsoft::UI::Input::KeyEventArgs const &args);
#endif

  int32_t OriginalSource() noexcept;
  winrt::hstring DeviceId() noexcept;
  bool Handled() noexcept;
  void Handled(bool value) noexcept;
  winrt::Windows::System::VirtualKey Key() noexcept;
  winrt::Windows::UI::Core::CorePhysicalKeyStatus KeyStatus() noexcept;
  winrt::Windows::System::VirtualKey OriginalKey() noexcept;

 private:
  facebook::react::Tag m_tag{-1};
  bool m_handled{false};
  winrt::Windows::System::VirtualKey m_key;
  winrt::Windows::UI::Core::CorePhysicalKeyStatus m_keyStatus;
};

struct PointerPointProperties : PointerPointPropertiesT<PointerPointProperties> {
#ifdef USE_WINUI3
  PointerPointProperties(const winrt::Microsoft::UI::Input::PointerPointProperties &ppp);
#endif

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
#ifdef USE_WINUI3
  winrt::Microsoft::UI::Input::PointerPointProperties m_sysPointerPointProps{nullptr};
#endif

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
#ifdef USE_WINUI3
  PointerPoint(const winrt::Microsoft::UI::Input::PointerPoint &pp);
#endif
  PointerPoint(uint32_t msg, uint64_t wParam, int64_t lParam, float scaleFactor);

  uint32_t FrameId() noexcept;
  bool IsInContact() noexcept;
  PointerDeviceType PointerDeviceType() noexcept;
  uint32_t PointerId() noexcept;
  winrt::Windows::Foundation::Point Position() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPointProperties Properties() noexcept;
  uint64_t Timestamp() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint GetTransformedPoint(
      const IPointerPointTransform &transform) noexcept;

 private:
  bool IsPointerMessage(uint32_t message) noexcept;

  // Windows::Input
#ifdef USE_WINUI3
  winrt::Microsoft::UI::Input::PointerPoint m_sysPointerPoint{nullptr};
#endif
  // WM_POINTER*
  POINTER_INFO m_pi = {0};
  // WM_*MOUSE
  uint32_t m_msg;
  uint64_t m_wParam;
  int64_t m_lParam;
  float m_scaleFactor;
};

struct PointerRoutedEventArgs : PointerRoutedEventArgsT<PointerRoutedEventArgs> {
  PointerRoutedEventArgs(
      facebook::react::Tag tag,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pp,
      const winrt::Windows::System::VirtualKeyModifiers &virtualKeyModifiers);

  int32_t OriginalSource() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint GetCurrentPoint(int32_t tag) noexcept;
  bool Handled() noexcept;
  void Handled(bool value) noexcept;
  winrt::Windows::System::VirtualKeyModifiers KeyModifiers() noexcept;

 private:
  facebook::react::Tag m_tag{-1};
  bool m_handled{false};
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint m_pointerPoint{nullptr};
  winrt::Windows::System::VirtualKeyModifiers m_virtualKeyModifiers;
};

} // namespace winrt::Microsoft::ReactNative::Composition::Input::implementation
