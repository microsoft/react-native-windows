// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ReactPointerEventArgs.g.h"
#include <UI.Xaml.Input.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactPointerEventArgs : ReactPointerEventArgsT<ReactPointerEventArgs> {
  xaml::Input::PointerRoutedEventArgs Args() const noexcept;

  PointerEventKind Kind() const noexcept;
  void Kind(PointerEventKind kind) noexcept;

  winrt::IInspectable Target() const noexcept;
  void Target(winrt::IInspectable const &target) noexcept;

  // Internal use
  ReactPointerEventArgs(PointerEventKind kind, xaml::Input::PointerRoutedEventArgs const &args) noexcept;

 private:
  PointerEventKind m_kind;
  xaml::Input::PointerRoutedEventArgs const &m_args;
  winrt::IInspectable m_target{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation
