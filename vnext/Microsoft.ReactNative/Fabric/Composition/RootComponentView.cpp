
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "RootComponentView.h"

#include <Fabric/FabricUIManagerModule.h>
#include "CompositionRootAutomationProvider.h"
#include "CompositionRootView.h"
#include "Theme.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

RootComponentView::RootComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default &
              ~(ComponentViewFeatures::Background | ComponentViewFeatures::ShadowProps |
                ComponentViewFeatures::NativeBorder),
          false) {}

winrt::Microsoft::ReactNative::ComponentView RootComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<RootComponentView>(compContext, tag, reactContext);
}

RootComponentView *RootComponentView::rootComponentView() noexcept {
  return this;
}

winrt::Microsoft::ReactNative::ComponentView &RootComponentView::GetFocusedComponent() noexcept {
  return m_focusedComponent;
}
void RootComponentView::SetFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &value) noexcept {
  if (m_focusedComponent == value)
    return;

  if (m_focusedComponent) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_focusedComponent)->onFocusLost();
  }

  if (value)
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(value)->onFocusGained();

  m_focusedComponent = value;
}

winrt::Microsoft::ReactNative::implementation::ComponentView *NavigateFocusHelper(
    winrt::Microsoft::ReactNative::implementation::ComponentView &view,
    winrt::Microsoft::ReactNative::FocusNavigationReason reason) {
  if (reason == winrt::Microsoft::ReactNative::FocusNavigationReason::First) {
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
    winrt::Microsoft::ReactNative::ComponentView component{nullptr};
    winrt::check_hresult(view->QueryInterface(
        winrt::guid_of<winrt::Microsoft::ReactNative::ComponentView>(), winrt::put_abi(component)));
    SetFocusedComponent(component);
  }
  return view != nullptr;
}

bool RootComponentView::TrySetFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  auto selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view);
  if (selfView->focusable()) {
    selfView->rootComponentView()->SetFocusedComponent(view);
    return true;
  }
  return false;
}

bool RootComponentView::TryMoveFocus(bool next) noexcept {
  if (!m_focusedComponent) {
    return NavigateFocus(winrt::Microsoft::ReactNative::FocusNavigationRequest(
        next ? winrt::Microsoft::ReactNative::FocusNavigationReason::First
             : winrt::Microsoft::ReactNative::FocusNavigationReason::Last));
  }

  Mso::Functor<bool(const winrt::Microsoft::ReactNative::ComponentView &)> fn =
      [currentlyFocused = m_focusedComponent](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
        if (view == currentlyFocused)
          return false;

        return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view)
            ->rootComponentView()
            ->TrySetFocusedComponent(view);
      };

  return winrt::Microsoft::ReactNative::implementation::walkTree(m_focusedComponent, next, fn);
}

HRESULT RootComponentView::GetFragmentRoot(IRawElementProviderFragmentRoot **pRetVal) noexcept {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto rootView = uiManager->GetCompositionRootView(Tag());
  if (!rootView) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  auto spFragmentRoot = rootView.GetUiaProvider().as<IRawElementProviderFragmentRoot>();
  if (spFragmentRoot) {
    *pRetVal = spFragmentRoot.detach();
  }

  return S_OK;
}

winrt::IInspectable RootComponentView::UiaProviderFromPoint(const POINT &ptPixels) noexcept {
  facebook::react::Point ptDips{
      static_cast<facebook::react::Float>(ptPixels.x) / m_layoutMetrics.pointScaleFactor,
      static_cast<facebook::react::Float>(ptPixels.y) / m_layoutMetrics.pointScaleFactor};

  facebook::react::Point localPt;
  auto tag = hitTest(ptDips, localPt, true);

  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return nullptr;

  auto view = uiManager->GetViewRegistry().findComponentViewWithTag(tag);
  if (view == nullptr)
    return nullptr;

  return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view)->EnsureUiaProvider();
}

winrt::Microsoft::ReactNative::implementation::ClipState RootComponentView::getClipState() noexcept {
  return winrt::Microsoft::ReactNative::implementation::ClipState::NoClip;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
