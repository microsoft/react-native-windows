// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPointerEventArgs.g.cpp"
#include "ReactPointerEventArgs.h"

namespace winrt::Microsoft::ReactNative::implementation {

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

void ReactPointerEventArgs::StopPropagation() {
  m_propagationStopped = true;
}

ReactPointerEventArgs::ReactPointerEventArgs(xaml::Input::PointerRoutedEventArgs const &args) noexcept
    : m_args{args} {}

bool ReactPointerEventArgs::CaptureReleased() const noexcept {
  return m_captureReleased;
}

bool ReactPointerEventArgs::DefaultPrevented() const noexcept {
  return m_defaultPrevented;
}

bool ReactPointerEventArgs::PropagationStopped() const noexcept {
  return m_propagationStopped;
}

} // namespace winrt::Microsoft::ReactNative::implementation
