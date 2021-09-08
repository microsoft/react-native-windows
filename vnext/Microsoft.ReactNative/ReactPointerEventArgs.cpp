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

void ReactPointerEventArgs::AllowUncaptured() {
  m_uncapturedAllowed = true;
}

void ReactPointerEventArgs::PreventDefault() {
  m_defaultPrevented = true;
}

ReactPointerEventArgs::ReactPointerEventArgs(
    PointerEventKind kind,
    xaml::Input::PointerRoutedEventArgs const &args) noexcept
    : m_kind{kind}, m_args{args} {}

bool ReactPointerEventArgs::DefaultPrevented() const noexcept {
  return m_defaultPrevented;
}

bool ReactPointerEventArgs::UncapturedAllowed() const noexcept {
  return m_uncapturedAllowed;
}

} // namespace winrt::Microsoft::ReactNative::implementation
