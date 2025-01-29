// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.FocusManager.g.h"
#include "GettingFocusEventArgs.g.h"
#include "LosingFocusEventArgs.g.h"
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct LostFocusEventArgs
    : winrt::implements<LostFocusEventArgs, winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  LostFocusEventArgs(const winrt::Microsoft::ReactNative::ComponentView &originalSource);
  int32_t OriginalSource() noexcept;

 private:
  const int32_t m_originalSource;
};

struct GotFocusEventArgs
    : winrt::implements<GotFocusEventArgs, winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  GotFocusEventArgs(
      const winrt::Microsoft::ReactNative::ComponentView &originalSource,
      winrt::Microsoft::ReactNative::FocusNavigationDirection direction);
  int32_t OriginalSource() noexcept;

  winrt::Microsoft::ReactNative::FocusNavigationDirection Direction() const noexcept {
    return m_direction;
  }

 private:
  const int32_t m_originalSource;
  winrt::Microsoft::ReactNative::FocusNavigationDirection m_direction{
      winrt::Microsoft::ReactNative::FocusNavigationDirection::None};
};

struct LosingFocusEventArgs
    : winrt::Microsoft::ReactNative::implementation::LosingFocusEventArgsT<LosingFocusEventArgs> {
  LosingFocusEventArgs(
      const winrt::Microsoft::ReactNative::ComponentView &originalSource,
      winrt::Microsoft::ReactNative::FocusNavigationDirection direction,
      const winrt::Microsoft::ReactNative::ComponentView &oldFocusedComponent,
      const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent);
  int32_t OriginalSource() const noexcept;
  winrt::Microsoft::ReactNative::FocusNavigationDirection Direction() const noexcept;

  winrt::Microsoft::ReactNative::ComponentView NewFocusedComponent() const noexcept;
  winrt::Microsoft::ReactNative::ComponentView OldFocusedComponent() const noexcept;

  void TryCancel() noexcept;
  void TrySetNewFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent) noexcept;

 private:
  const int32_t m_originalSource;
  const winrt::Microsoft::ReactNative::FocusNavigationDirection m_direction;
  winrt::Microsoft::ReactNative::ComponentView m_old{nullptr};
  winrt::Microsoft::ReactNative::ComponentView m_new{nullptr};
};

struct GettingFocusEventArgs
    : winrt::Microsoft::ReactNative::implementation::GettingFocusEventArgsT<GettingFocusEventArgs> {
  GettingFocusEventArgs(
      const winrt::Microsoft::ReactNative::ComponentView &originalSource,
      winrt::Microsoft::ReactNative::FocusNavigationDirection direction,
      const winrt::Microsoft::ReactNative::ComponentView &oldFocusedComponent,
      const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent);
  int32_t OriginalSource() const noexcept;
  winrt::Microsoft::ReactNative::FocusNavigationDirection Direction() const noexcept;
  winrt::Microsoft::ReactNative::ComponentView NewFocusedComponent() const noexcept;
  winrt::Microsoft::ReactNative::ComponentView OldFocusedComponent() const noexcept;

  void TryCancel() noexcept;
  void TrySetNewFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent) noexcept;

 private:
  const int32_t m_originalSource;
  const winrt::Microsoft::ReactNative::FocusNavigationDirection m_direction;
  winrt::Microsoft::ReactNative::ComponentView m_old{nullptr};
  winrt::Microsoft::ReactNative::ComponentView m_new{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct FocusManager : FocusManagerT<FocusManager> {
  FocusManager() = default;

  static winrt::Microsoft::ReactNative::ComponentView FindFirstFocusableElement(
      const winrt::Microsoft::ReactNative::ComponentView &searchScope) noexcept;
  static winrt::Microsoft::ReactNative::ComponentView FindLastFocusableElement(
      const winrt::Microsoft::ReactNative::ComponentView &searchScope) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {
struct FocusManager : FocusManagerT<FocusManager, implementation::FocusManager> {};
} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
