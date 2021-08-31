// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ReactPointerEventArgs.g.h"
#include <UI.Xaml.Input.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactPointerEventArgs : ReactPointerEventArgsT<ReactPointerEventArgs> {
  PointerEventKind Kind() const noexcept;
  xaml::Input::PointerRoutedEventArgs Args() const noexcept;

  winrt::IInspectable Target() const noexcept;
  void Target(winrt::IInspectable const &target) noexcept;

  void PreventDefault();
  void ReleaseCapture();
  void StopPropagation();

  // Internal use
  ReactPointerEventArgs(PointerEventKind kind, xaml::Input::PointerRoutedEventArgs const &args) noexcept;

  bool CaptureReleased() const noexcept;
  bool DefaultPrevented() const noexcept;
  bool PropagationStopped() const noexcept;

 private:
  PointerEventKind m_kind;
  xaml::Input::PointerRoutedEventArgs const &m_args;
  winrt::IInspectable m_target{nullptr};

  bool m_captureReleased{false};
  bool m_defaultPrevented{false};
  bool m_propagationStopped{false};
};

} // namespace winrt::Microsoft::ReactNative::implementation
