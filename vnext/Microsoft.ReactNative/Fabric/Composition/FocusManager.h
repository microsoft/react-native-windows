// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.FocusManager.g.h"
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include "LosingFocusEventArgs.g.h"
#include "GettingFocusEventArgs.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct LostFocusEventArgs
    : winrt::implements<LostFocusEventArgs, winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  LostFocusEventArgs(const winrt::Microsoft::ReactNative::ComponentView& originalSource);
  int32_t OriginalSource() noexcept;

 private:
  const int32_t m_originalSource;
};

struct GotFocusEventArgs
    : winrt::implements<GotFocusEventArgs, winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> {
  GotFocusEventArgs(const winrt::Microsoft::ReactNative::ComponentView& originalSource);
  int32_t OriginalSource() noexcept;

 private:
  const int32_t m_originalSource;
};

struct LosingFocusEventArgs
    : winrt::Microsoft::ReactNative::implementation::LosingFocusEventArgsT<LosingFocusEventArgs> {
  LosingFocusEventArgs(
      const winrt::Microsoft::ReactNative::ComponentView& originalSource,
      const winrt::Microsoft::ReactNative::ComponentView &oldFocusedComponent,
      const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent);
  int32_t OriginalSource() noexcept;
  winrt::Microsoft::ReactNative::ComponentView NewFocusedComponent() noexcept;
  winrt::Microsoft::ReactNative::ComponentView OldFocusedComponent() noexcept;

  void TryCancel() noexcept;
  void TrySetNewFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent) noexcept;

 private:
  const int32_t m_originalSource;
  winrt::Microsoft::ReactNative::ComponentView m_old{nullptr};
  winrt::Microsoft::ReactNative::ComponentView m_new{nullptr};
};

struct GettingFocusEventArgs
    : winrt::Microsoft::ReactNative::implementation::GettingFocusEventArgsT<GettingFocusEventArgs> {
  GettingFocusEventArgs(
      const winrt::Microsoft::ReactNative::ComponentView& originalSource,
      const winrt::Microsoft::ReactNative::ComponentView &oldFocusedComponent,
      const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent);
  int32_t OriginalSource() noexcept;
  winrt::Microsoft::ReactNative::ComponentView NewFocusedComponent() noexcept;
  winrt::Microsoft::ReactNative::ComponentView OldFocusedComponent() noexcept;

  void TryCancel() noexcept;
  void TrySetNewFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent) noexcept;

 private:
  const int32_t m_originalSource;
  winrt::Microsoft::ReactNative::ComponentView m_old{nullptr};
  winrt::Microsoft::ReactNative::ComponentView m_new{nullptr};
};
}

namespace winrt::Microsoft::ReactNative::Composition::implementation {


struct FocusManager : FocusManagerT<FocusManager> {
  FocusManager() = default;

  static winrt::Microsoft::ReactNative::ComponentView FindFirstFocusableElement(const winrt::Microsoft::ReactNative::ComponentView& searchScope) noexcept;
  static winrt::Microsoft::ReactNative::ComponentView FindLastFocusableElement(const winrt::Microsoft::ReactNative::ComponentView& searchScope) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {
struct FocusManager : FocusManagerT<FocusManager, implementation::FocusManager> {};
} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
