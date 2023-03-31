
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "RootComponentView.h"

#include "CompositionRootAutomationProvider.h"
#include "CompositionRootView.h"

namespace Microsoft::ReactNative {

RootComponentView::RootComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {}

std::shared_ptr<RootComponentView> RootComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag) noexcept {
  return std::shared_ptr<RootComponentView>(new RootComponentView(compContext, tag));
}

RootComponentView *RootComponentView::rootComponentView() noexcept {
  return this;
}

IComponentView *RootComponentView::GetFocusedComponent() noexcept {
  return m_focusedComponent;
}
void RootComponentView::SetFocusedComponent(IComponentView *value) noexcept {
  if (m_focusedComponent == value)
    return;

  if (m_focusedComponent) {
    m_focusedComponent->onFocusLost();
  }

  if (value)
    value->onFocusGained();

  m_focusedComponent = value;
}

IComponentView *NavigateFocusHelper(IComponentView &view, winrt::Microsoft::ReactNative::FocusNavigationReason reason) {
  if (reason == winrt::Microsoft::ReactNative::FocusNavigationReason::First) {
    if (view.focusable()) {
      return &view;
    }
  }
  IComponentView *toFocus = nullptr;

  Mso::Functor<bool(::Microsoft::ReactNative::IComponentView & v)> fn =
      [reason, &toFocus](::Microsoft::ReactNative::IComponentView &v) noexcept
      -> bool { return (toFocus = NavigateFocusHelper(v, reason)); };

  if (view.runOnChildren(reason == winrt::Microsoft::ReactNative::FocusNavigationReason::First, fn)) {
    return toFocus;
  }

  if (reason == winrt::Microsoft::ReactNative::FocusNavigationReason::Last) {
    if (view.focusable()) {
      return &view;
    }
  }

  return nullptr;
}

bool RootComponentView::NavigateFocus(const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept {
  if (request.Reason() == winrt::Microsoft::ReactNative::FocusNavigationReason::Restore) {
    // No-op for now
    return m_focusedComponent != nullptr;
  }

  auto view = NavigateFocusHelper(*this, request.Reason());
  if (view) {
    SetFocusedComponent(view);
  }
  return view != nullptr;
}

bool RootComponentView::TryMoveFocus(bool next) noexcept {
  if (!m_focusedComponent) {
    return NavigateFocus(winrt::Microsoft::ReactNative::FocusNavigationRequest(
        next ? winrt::Microsoft::ReactNative::FocusNavigationReason::First
             : winrt::Microsoft::ReactNative::FocusNavigationReason::Last));
  }

  Mso::Functor<bool(IComponentView &)> fn = [currentlyFocused = m_focusedComponent](IComponentView &view) noexcept {
    if (&view == currentlyFocused)
      return false;
    if (view.focusable()) {
      view.rootComponentView()->SetFocusedComponent(&view);
      return true;
    }
    return false;
  };

  return walkTree(*m_focusedComponent, next, fn);
}

winrt::IInspectable RootComponentView::EnsureUiaProvider() noexcept {
  if (m_uiaProvider == nullptr) {
    m_uiaProvider =
        winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionRootAutomationProvider>(getPtr());
  }
  return m_uiaProvider;
}

std::shared_ptr<RootComponentView> RootComponentView::getPtr() {
  return std::static_pointer_cast<RootComponentView>(shared_from_this());
}

} // namespace Microsoft::ReactNative
