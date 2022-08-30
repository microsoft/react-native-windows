// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPointerEventArgs.h"
#include "ReactPointerEventArgs.g.cpp"

namespace winrt::Microsoft::ReactNative::implementation {

xaml::Input::PointerRoutedEventArgs ReactPointerEventArgs::Args() const noexcept {
  return m_args;
}

PointerEventKind ReactPointerEventArgs::Kind() const noexcept {
  return m_kind;
}

void ReactPointerEventArgs::Kind(PointerEventKind kind) noexcept {
  // The only event type transitions that are supported are:
  // 1. PointerEventKind::CaptureLost to PointerEventKind::End to support cases
  //    where XAML firing the CaptureLost event should be treated as a gesture
  //    completion event, e.g., for pointer events in selectable text that do
  //    not result in text selection.
  // 2. PointerEventKind::End to PointerEventKind::Cancel to support cases
  //    where a custom view manager implementing `IViewManagerWithPointerEvents`
  //    wants to cancel a gesture, e.g., for a custom drag handle where the end
  //    of the gesture should not be treated as a completed press event.
  assert(
      (kind == PointerEventKind::End && m_kind == PointerEventKind::CaptureLost) ||
      (kind == PointerEventKind::Cancel && m_kind == PointerEventKind::End));
  m_kind = kind;
}

winrt::IInspectable ReactPointerEventArgs::Target() const noexcept {
  return m_target;
}

void ReactPointerEventArgs::Target(winrt::IInspectable const &target) noexcept {
  m_target = target;
}

ReactPointerEventArgs::ReactPointerEventArgs(
    PointerEventKind kind,
    xaml::Input::PointerRoutedEventArgs const &args) noexcept
    : m_kind{kind}, m_args{args} {}

} // namespace winrt::Microsoft::ReactNative::implementation
