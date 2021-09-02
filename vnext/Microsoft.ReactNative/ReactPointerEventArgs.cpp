// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPointerEventArgs.h"
#include "ReactPointerEventArgs.g.cpp"

namespace winrt::Microsoft::ReactNative::implementation {

PointerEventKind ReactPointerEventArgs::Kind() const noexcept {
  return m_kind;
}

xaml::Input::PointerRoutedEventArgs ReactPointerEventArgs::Args() const noexcept {
  return m_args;
}

winrt::IInspectable ReactPointerEventArgs::Target() const noexcept {
  return m_target;
}

void ReactPointerEventArgs::Target(winrt::IInspectable const &target) noexcept {
  m_target = target;
}

void ReactPointerEventArgs::PreventDefault() {
  m_defaultPrevented = true;
}

void ReactPointerEventArgs::ReleaseCapture() {
  m_captureReleased = true;
}

ReactPointerEventArgs::ReactPointerEventArgs(
    PointerEventKind kind,
    xaml::Input::PointerRoutedEventArgs const &args) noexcept
    : m_kind{kind}, m_args{args} {}

bool ReactPointerEventArgs::CaptureReleased() const noexcept {
  return m_captureReleased;
}

bool ReactPointerEventArgs::DefaultPrevented() const noexcept {
  return m_defaultPrevented;
}

} // namespace winrt::Microsoft::ReactNative::implementation
