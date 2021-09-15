// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ReactPointerEventArgs.g.h"
#include <UI.Xaml.Input.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactPointerEventArgs : ReactPointerEventArgsT<ReactPointerEventArgs> {
  xaml::Input::PointerRoutedEventArgs Args() const noexcept;

  PointerEventKind Kind() const noexcept;
  void Kind(PointerEventKind const &kind) noexcept;

  winrt::IInspectable Target() const noexcept;
  void Target(winrt::IInspectable const &target) noexcept;

  void AllowUncaptured();
  void PreventDefault();

  // Internal use
  ReactPointerEventArgs(PointerEventKind kind, xaml::Input::PointerRoutedEventArgs const &args) noexcept;

  bool UncapturedAllowed() const noexcept;
  bool DefaultPrevented() const noexcept;

 private:
  PointerEventKind m_originalKind;
  std::optional<PointerEventKind> m_kind{std::nullopt};
  xaml::Input::PointerRoutedEventArgs const &m_args;
  winrt::IInspectable m_target{nullptr};

  bool m_uncapturedAllowed{false};
  bool m_defaultPrevented{false};
};

} // namespace winrt::Microsoft::ReactNative::implementation
