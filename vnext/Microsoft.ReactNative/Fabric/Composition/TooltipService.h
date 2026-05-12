
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ReactTaggedView.h>
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative {

struct TooltipService;

struct TooltipTracker {
  TooltipTracker(
      const winrt::Microsoft::ReactNative::ComponentView &view,
      const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
      TooltipService *outer);
  ~TooltipTracker();

  void OnPointerEntered(
      const winrt::Windows::Foundation::IInspectable &sender,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnPointerMoved(
      const winrt::Windows::Foundation::IInspectable &sender,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnPointerExited(
      const winrt::Windows::Foundation::IInspectable &sender,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnGotFocus(
      const winrt::Windows::Foundation::IInspectable &sender,
      const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept;
  void OnLostFocus(
      const winrt::Windows::Foundation::IInspectable &sender,
      const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept;
  void OnTick(
      const winrt::Windows::Foundation::IInspectable &,
      const winrt::Windows::Foundation::IInspectable &) noexcept;
  void OnUnmounted(
      const winrt::Windows::Foundation::IInspectable &,
      const winrt::Microsoft::ReactNative::ComponentView &) noexcept;

  facebook::react::Tag Tag() const noexcept;

  // Cancel pending dwell timer and close any visible tooltip popup; used by the service when another tracker takes over.
  void DismissForExternalRequest() noexcept;

 private:
  void ShowTooltip(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept;
  void DestroyTimer() noexcept;
  void DestroyTooltip() noexcept;

  TooltipService *m_outer;
  winrt::Windows::Foundation::Point m_pos;
  ::Microsoft::ReactNative::ReactTaggedView m_view;
  winrt::Microsoft::ReactNative::ITimer m_timer;
  HWND m_hwndTip{nullptr};
  bool m_focusTooltip{false};
  winrt::Microsoft::ReactNative::ReactPropertyBag m_properties;
};

struct TooltipService {
  TooltipService(const winrt::Microsoft::ReactNative::ReactPropertyBag &properties);
  void StartTracking(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept;
  void StopTracking(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept;

  // Enforce "only one tooltip visible at a time": dismisses the previously active tracker, if any.
  void NotifyShow(TooltipTracker *tracker) noexcept;
  // Clears the active-tracker slot if it still points at `tracker`.
  void NotifyDismiss(TooltipTracker *tracker) noexcept;

  static std::shared_ptr<TooltipService> GetCurrent(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &properties) noexcept;

 private:
  std::vector<std::shared_ptr<TooltipTracker>> m_enteredTrackers;
  std::vector<std::shared_ptr<TooltipTracker>> m_trackers;
  TooltipTracker *m_activeTracker{nullptr};
  winrt::Microsoft::ReactNative::ReactPropertyBag m_properties;
};

} // namespace winrt::Microsoft::ReactNative
