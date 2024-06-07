
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "RootComponentView.h"

#include <Fabric/FabricUIManagerModule.h>
#include "CompositionRootAutomationProvider.h"
#include "ReactNativeIsland.h"
#include "Theme.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

RootComponentView::RootComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          {}, // default viewProps
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default &
              ~(ComponentViewFeatures::Background | ComponentViewFeatures::ShadowProps |
                ComponentViewFeatures::NativeBorder),
          false) {}

RootComponentView::~RootComponentView() {
  if (auto rootView = m_wkRootView.get()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->RemoveRenderedVisual(
        OuterVisual());
  }
}

winrt::Microsoft::ReactNative::ComponentView RootComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<RootComponentView>(compContext, tag, reactContext);
}

RootComponentView *RootComponentView::rootComponentView() noexcept {
  return this;
}

void RootComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);

  if (oldLayoutMetrics.frame != layoutMetrics.frame ||
      oldLayoutMetrics.pointScaleFactor != layoutMetrics.pointScaleFactor) {
    if (auto rootView = m_wkRootView.get()) {
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->NotifySizeChanged();
    }
  }
}

winrt::Microsoft::ReactNative::ComponentView RootComponentView::GetFocusedComponent() noexcept {
  return m_focusedComponent;
}
void RootComponentView::SetFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &value) noexcept {
  if (m_focusedComponent == value)
    return;

  if (m_focusedComponent) {
    auto args = winrt::make<winrt::Microsoft::ReactNative::implementation::LostFocusEventArgs>(m_focusedComponent);
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_focusedComponent)
        ->onLostFocus(args);
  }

  if (value) {
    if (auto rootView = m_wkRootView.get()) {
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->TrySetFocus();
    }
    auto args = winrt::make<winrt::Microsoft::ReactNative::implementation::GotFocusEventArgs>(value);
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(value)->onGotFocus(args);
  }

  m_focusedComponent = value;
}

bool RootComponentView::NavigateFocus(const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept {
  if (request.Reason() == winrt::Microsoft::ReactNative::FocusNavigationReason::Restore) {
    // No-op for now
    return m_focusedComponent != nullptr;
  }

  auto view = (request.Reason() == winrt::Microsoft::ReactNative::FocusNavigationReason::First)
      ? FocusManager::FindFirstFocusableElement(*this)
      : FocusManager::FindLastFocusableElement(*this);
  if (view) {
    TrySetFocusedComponent(view);
  }
  return view != nullptr;
}

bool RootComponentView::TrySetFocusedComponent(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  auto target = view;
  auto selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(target);
  if (selfView && !selfView->focusable()) {
    target = FocusManager::FindFirstFocusableElement(target);
    if (!target)
      return false;
    selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(target);
  }

  if (selfView && selfView->rootComponentView() != this)
    return false;

  auto losingFocusArgs = winrt::make<winrt::Microsoft::ReactNative::implementation::LosingFocusEventArgs>(
      target, m_focusedComponent, target);
  if (m_focusedComponent) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_focusedComponent)
        ->onLosingFocus(losingFocusArgs);
  }

  if (losingFocusArgs.NewFocusedComponent()) {
    auto gettingFocusArgs = winrt::make<winrt::Microsoft::ReactNative::implementation::GettingFocusEventArgs>(
        target, m_focusedComponent, losingFocusArgs.NewFocusedComponent());
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(losingFocusArgs.NewFocusedComponent())
        ->onGettingFocus(gettingFocusArgs);

    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(losingFocusArgs.NewFocusedComponent())
        ->rootComponentView()
        ->SetFocusedComponent(gettingFocusArgs.NewFocusedComponent());
  }

  return true;
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

  auto rootView{uiManager->GetReactNativeIsland(Tag())};
  if (!rootView) {
    return UIA_E_ELEMENTNOTAVAILABLE;
  }

  auto spFragmentRoot = rootView.GetUiaProvider().as<IRawElementProviderFragmentRoot>();
  if (spFragmentRoot) {
    *pRetVal = spFragmentRoot.detach();
  }

  return S_OK;
}

uint32_t RootComponentView::overlayIndex() noexcept {
  return 1;
}

void RootComponentView::start(const winrt::Microsoft::ReactNative::ReactNativeIsland &rootView) noexcept {
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->AddRenderedVisual(
      OuterVisual());
  m_wkRootView = rootView;
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
