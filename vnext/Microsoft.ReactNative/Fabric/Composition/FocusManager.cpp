// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "FocusManager.h"
#include "Composition.FocusManager.g.cpp"
#include <Fabric/FabricUIManagerModule.h>
#include <limits>

namespace winrt::Microsoft::ReactNative::implementation {

LostFocusEventArgs::LostFocusEventArgs(const winrt::Microsoft::ReactNative::ComponentView &originalSource)
    : m_originalSource(originalSource ? originalSource.Tag() : -1) {}
int32_t LostFocusEventArgs::OriginalSource() noexcept {
  return m_originalSource;
}

GotFocusEventArgs::GotFocusEventArgs(
    const winrt::Microsoft::ReactNative::ComponentView &originalSource,
    winrt::Microsoft::ReactNative::FocusNavigationDirection direction)
    : m_originalSource(originalSource ? originalSource.Tag() : -1), m_direction(direction) {}
int32_t GotFocusEventArgs::OriginalSource() noexcept {
  return m_originalSource;
}

LosingFocusEventArgs::LosingFocusEventArgs(
    const winrt::Microsoft::ReactNative::ComponentView &originalSource,
    winrt::Microsoft::ReactNative::FocusNavigationDirection direction,
    const winrt::Microsoft::ReactNative::ComponentView &oldFocusedComponent,
    const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent)
    : m_originalSource(originalSource ? originalSource.Tag() : -1),
      m_direction(direction),
      m_old(oldFocusedComponent),
      m_new(newFocusedComponent) {}

int32_t LosingFocusEventArgs::OriginalSource() const noexcept {
  return m_originalSource;
}

winrt::Microsoft::ReactNative::FocusNavigationDirection LosingFocusEventArgs::Direction() const noexcept {
  return m_direction;
}

winrt::Microsoft::ReactNative::ComponentView LosingFocusEventArgs::NewFocusedComponent() const noexcept {
  return m_new;
}
winrt::Microsoft::ReactNative::ComponentView LosingFocusEventArgs::OldFocusedComponent() const noexcept {
  return m_old;
}

void LosingFocusEventArgs::TryCancel() noexcept {
  m_new = m_old;
}

void LosingFocusEventArgs::TrySetNewFocusedComponent(
    const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent) noexcept {
  auto selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(newFocusedComponent);
  if (selfView->focusable()) {
    m_new = newFocusedComponent;
  } else {
    auto target =
        winrt::Microsoft::ReactNative::Composition::FocusManager::FindFirstFocusableElement(newFocusedComponent);
    if (!target)
      return;
    m_new = target;
  }
}

GettingFocusEventArgs::GettingFocusEventArgs(
    const winrt::Microsoft::ReactNative::ComponentView &originalSource,
    winrt::Microsoft::ReactNative::FocusNavigationDirection direction,
    const winrt::Microsoft::ReactNative::ComponentView &oldFocusedComponent,
    const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent)
    : m_originalSource(originalSource ? originalSource.Tag() : -1),
      m_direction(direction),
      m_old(oldFocusedComponent),
      m_new(newFocusedComponent) {}

int32_t GettingFocusEventArgs::OriginalSource() const noexcept {
  return m_originalSource;
}

winrt::Microsoft::ReactNative::FocusNavigationDirection GettingFocusEventArgs::Direction() const noexcept {
  return m_direction;
}

winrt::Microsoft::ReactNative::ComponentView GettingFocusEventArgs::NewFocusedComponent() const noexcept {
  return m_new;
}
winrt::Microsoft::ReactNative::ComponentView GettingFocusEventArgs::OldFocusedComponent() const noexcept {
  return m_old;
}

void GettingFocusEventArgs::TryCancel() noexcept {
  m_new = m_old;
}

void GettingFocusEventArgs::TrySetNewFocusedComponent(
    const winrt::Microsoft::ReactNative::ComponentView &newFocusedComponent) noexcept {
  auto selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(newFocusedComponent);
  if (selfView->focusable()) {
    m_new = newFocusedComponent;
  } else {
    auto target =
        winrt::Microsoft::ReactNative::Composition::FocusManager::FindFirstFocusableElement(newFocusedComponent);
    if (!target)
      return;
    m_new = target;
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::Composition::implementation {

winrt::Microsoft::ReactNative::implementation::ComponentView *NavigateFocusHelper(
    winrt::Microsoft::ReactNative::implementation::ComponentView &view,
    winrt::Microsoft::ReactNative::FocusNavigationReason reason) {
  if (reason == winrt::Microsoft::ReactNative::FocusNavigationReason::First) {
    // For First navigation, we need to consider tabIndex
    auto tabIndex = view.tabIndex();
    
    // If this view has a positive tabIndex, it should be focusable
    if (tabIndex >= 0 && tabIndex != std::numeric_limits<int>::max()) {
      return &view;
    }
    
    // Otherwise, check if it's naturally focusable
    if (view.focusable()) {
      return &view;
    }
  }
  
  winrt::Microsoft::ReactNative::implementation::ComponentView *toFocus = nullptr;

  Mso::Functor<bool(::winrt::Microsoft::ReactNative::implementation::ComponentView & v)> fn =
      [reason, &toFocus](::winrt::Microsoft::ReactNative::implementation::ComponentView &v) noexcept
      -> bool { return (toFocus = NavigateFocusHelper(v, reason)); };

  if (view.runOnChildren(reason == winrt::Microsoft::ReactNative::FocusNavigationReason::First, fn)) {
    return toFocus;
  }

  if (reason == winrt::Microsoft::ReactNative::FocusNavigationReason::Last) {
    // For Last navigation, we need to consider tabIndex
    auto tabIndex = view.tabIndex();
    
    // If this view has a positive tabIndex, it should be focusable
    if (tabIndex >= 0 && tabIndex != std::numeric_limits<int>::max()) {
      return &view;
    }
    
    // Otherwise, check if it's naturally focusable
    if (view.focusable()) {
      return &view;
    }
  }

  return nullptr;
}

winrt::Microsoft::ReactNative::ComponentView FocusManager::FindFirstFocusableElement(
    const winrt::Microsoft::ReactNative::ComponentView &searchScope) noexcept {
  auto selfSearchScope = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(searchScope);
  auto view = NavigateFocusHelper(*selfSearchScope, winrt::Microsoft::ReactNative::FocusNavigationReason::First);
  if (view) {
    winrt::Microsoft::ReactNative::ComponentView component{nullptr};
    winrt::check_hresult(view->QueryInterface(
        winrt::guid_of<winrt::Microsoft::ReactNative::ComponentView>(), winrt::put_abi(component)));
    return *view;
  }
  return nullptr;
}

winrt::Microsoft::ReactNative::ComponentView FocusManager::FindLastFocusableElement(
    const winrt::Microsoft::ReactNative::ComponentView &searchScope) noexcept {
  auto selfSearchScope = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(searchScope);
  auto view = NavigateFocusHelper(*selfSearchScope, winrt::Microsoft::ReactNative::FocusNavigationReason::Last);
  if (view) {
    winrt::Microsoft::ReactNative::ComponentView component{nullptr};
    winrt::check_hresult(view->QueryInterface(
        winrt::guid_of<winrt::Microsoft::ReactNative::ComponentView>(), winrt::put_abi(component)));
    return *view;
  }
  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
