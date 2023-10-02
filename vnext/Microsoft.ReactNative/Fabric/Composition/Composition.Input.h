// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <react/renderer/core/ReactPrimitives.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include "Composition.Input.PointerPointProperties.g.h"
#include "Composition.Input.PointerPoint.g.h"
#include "Composition.Input.PointerRoutedEventArgs.g.h"

namespace winrt::Microsoft::ReactNative::Composition::Input::implementation {

struct KeyRoutedEventArgs : winrt::implements<
                                KeyRoutedEventArgs,
                                winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs,
                                winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  KeyRoutedEventArgs(facebook::react::Tag tag, uint32_t msg, uint64_t wParam, int64_t lParam);

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
  PointerPointProperties(const winrt::Windows::UI::Input::PointerPointProperties& ppp);

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
  winrt::Windows::UI::Input::PointerPointProperties m_systemPPP;
};

struct PointerPoint : PointerPointT<PointerPoint>
{
  PointerPoint(const winrt::Windows::UI::Input::PointerPoint& pp);

  uint32_t FrameId() noexcept;
  bool IsInContact() noexcept;
  PointerDeviceType PointerDeviceType() noexcept;
  uint32_t PointerId() noexcept;
  winrt::Windows::Foundation::Point Position() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPointProperties Properties() noexcept;
  uint64_t Timestamp() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint GetTransformedPoint(
    const IPointerPointTransform& transform) noexcept;

private:
  winrt::Windows::UI::Input::PointerPoint m_systemPP;
};

struct PointerRoutedEventArgs : PointerRoutedEventArgsT<PointerRoutedEventArgs>
{
  PointerRoutedEventArgs(facebook::react::Tag tag, const winrt::Windows::UI::Input::PointerPoint& pp);

  int32_t OriginalSource() noexcept;
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint GetCurrentPoint(int32_t tag) noexcept;
  bool Handled() noexcept;
  void Handled(bool value) noexcept;
  winrt::Windows::System::VirtualKeyModifiers KeyModifiers() noexcept;
 private:
  facebook::react::Tag m_tag{-1};
  bool m_handled{false};
  winrt::Microsoft::ReactNative::Composition::Input::PointerPoint m_pp{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::Input::implementation
