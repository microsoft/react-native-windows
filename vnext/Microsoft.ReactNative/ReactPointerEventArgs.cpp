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
  // The only event type change that is supported is CaptureLost to End.
  assert(kind == PointerEventKind::End && m_kind == PointerEventKind::CaptureLost);
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
