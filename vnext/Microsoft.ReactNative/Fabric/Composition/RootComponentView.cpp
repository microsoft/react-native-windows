
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "RootComponentView.h"

#include <Fabric/FabricUIManagerModule.h>
#include <winrt/Microsoft.UI.Input.h>
#include "CompositionRootAutomationProvider.h"
#include "ReactNativeIsland.h"
#include "Theme.h"
#include "CompositionDynamicAutomationProvider.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

RootComponentView::RootComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    ReactCompositionViewComponentBuilder *builder)
    : base_type(
          {}, // default viewProps
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default &
              ~(ComponentViewFeatures::Background | ComponentViewFeatures::ShadowProps |
                ComponentViewFeatures::NativeBorder | ComponentViewFeatures::FocusVisual),
          builder) {}

RootComponentView::RootComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portal,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          {}, // default viewProps
          compContext,
          -1,
          reactContext,
          ComponentViewFeatures::Default &
              ~(ComponentViewFeatures::Background | ComponentViewFeatures::ShadowProps |
                ComponentViewFeatures::NativeBorder | ComponentViewFeatures::FocusVisual),
          nullptr // builder,
          ),
      m_wkPortal(portal) {}

RootComponentView::~RootComponentView() {
  stop();
}

winrt::Microsoft::ReactNative::ComponentView RootComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<RootComponentView>(compContext, tag, reactContext, nullptr);
}

RootComponentView *RootComponentView::rootComponentView() const noexcept {
  return const_cast<RootComponentView *>(this);
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

void RootComponentView::SetFocusedComponent(
    const winrt::Microsoft::ReactNative::ComponentView &value,
    winrt::Microsoft::ReactNative::FocusNavigationDirection direction) noexcept {
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
    m_focusedComponent = value;
    auto args = winrt::make<winrt::Microsoft::ReactNative::implementation::GotFocusEventArgs>(value, direction);
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(value)->onGotFocus(args);
  }
}

bool RootComponentView::NavigateFocus(const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept {
  if (request.Reason() == winrt::Microsoft::ReactNative::FocusNavigationReason::Restore) {
    if (m_focusedComponent)
      return true;
  }

  auto view = (request.Reason() == winrt::Microsoft::ReactNative::FocusNavigationReason::Last)
      ? FocusManager::FindLastFocusableElement(*this)
      : FocusManager::FindFirstFocusableElement(*this);
  if (view) {
    TrySetFocusedComponent(
        view,
        request.Reason() == winrt::Microsoft::ReactNative::FocusNavigationReason::First
            ? winrt::Microsoft::ReactNative::FocusNavigationDirection::First
            : winrt::Microsoft::ReactNative::FocusNavigationDirection::Last);
  }
  return view != nullptr;
}

bool RootComponentView::TrySetFocusedComponent(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    winrt::Microsoft::ReactNative::FocusNavigationDirection direction,
    bool forceNoSelectionIfCannotMove /*= false*/) noexcept {
  auto target = view;
  auto selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(target);
  if (selfView && !selfView->focusable()) {
    target = (direction == winrt::Microsoft::ReactNative::FocusNavigationDirection::Last ||
              direction == winrt::Microsoft::ReactNative::FocusNavigationDirection::Previous)
        ? FocusManager::FindLastFocusableElement(target)
        : FocusManager::FindFirstFocusableElement(target);
    if (!target && !forceNoSelectionIfCannotMove)
      return false;
    selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(target);
  }

  if (selfView && selfView->rootComponentView() != this) {
    assert(false);
    return false;
  }

  auto losingFocusArgs = winrt::make<winrt::Microsoft::ReactNative::implementation::LosingFocusEventArgs>(
      target, direction, m_focusedComponent, target);
  if (m_focusedComponent) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_focusedComponent)
        ->onLosingFocus(losingFocusArgs);
  }

  if (losingFocusArgs.NewFocusedComponent()) {
    auto gettingFocusArgs = winrt::make<winrt::Microsoft::ReactNative::implementation::GettingFocusEventArgs>(
        target, direction, m_focusedComponent, losingFocusArgs.NewFocusedComponent());
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(losingFocusArgs.NewFocusedComponent())
        ->onGettingFocus(gettingFocusArgs);

    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(losingFocusArgs.NewFocusedComponent())
        ->rootComponentView()
        ->SetFocusedComponent(gettingFocusArgs.NewFocusedComponent(), direction);
  } else {
    SetFocusedComponent(nullptr, direction);
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
      [currentlyFocused = m_focusedComponent, next](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
        if (view == currentlyFocused)
          return false;
        auto selfView = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view);
        if (!selfView->focusable())
          return false;

        return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view)
            ->rootComponentView()
            ->TrySetFocusedComponent(
                view,
                next ? winrt::Microsoft::ReactNative::FocusNavigationDirection::Next
                     : winrt::Microsoft::ReactNative::FocusNavigationDirection::Previous);
      };

  if (winrt::Microsoft::ReactNative::implementation::walkTree(m_focusedComponent, next, fn)) {
    return true;
  }

  // We reached the end of the focus loop.  Notify the island in case the host wants to move focus somewhere outside the
  // island.
  auto island = parentContentIsland();
  if (island) {
    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(island);
    auto request = winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
        next ? winrt::Microsoft::UI::Input::FocusNavigationReason::Last
             : winrt::Microsoft::UI::Input::FocusNavigationReason::First);
    auto result = focusController.DepartFocus(request);
    if (result == winrt::Microsoft::UI::Input::FocusNavigationResult::Moved) {
      return true;
    }
  }

  // Wrap focus around if nothing outside the island takes focus
  return NavigateFocus(winrt::Microsoft::ReactNative::FocusNavigationRequest(
      next ? winrt::Microsoft::ReactNative::FocusNavigationReason::First
           : winrt::Microsoft::ReactNative::FocusNavigationReason::Last));
}

HRESULT RootComponentView::GetFragmentRoot(IRawElementProviderFragmentRoot **pRetVal) noexcept {
  if (pRetVal == nullptr)
    return E_POINTER;

  *pRetVal = nullptr;

  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto rootView = m_wkRootView.get();
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

void RootComponentView::start(const winrt::Microsoft::ReactNative::ReactNativeIsland &island) noexcept {
  theme(winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(island.Theme()));

  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(island)->AddRenderedVisual(
      OuterVisual());
  m_visualAddedToIsland = true;
  ReactNativeIsland(island);
}

void RootComponentView::stop() noexcept {
  SetFocusedComponent(nullptr, winrt::Microsoft::ReactNative::FocusNavigationDirection::None);
  if (m_visualAddedToIsland) {
    if (auto rootView = m_wkRootView.get()) {
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->RemoveRenderedVisual(
          OuterVisual());
    }
    m_visualAddedToIsland = false;
  }
  // Disconnect from the Island.  In case of an instance reload, the island may now
  // be attached to a new RootComponentView, so we should stop interacting with it.
  ReactNativeIsland(nullptr);
}

void RootComponentView::ReactNativeIsland(const winrt::Microsoft::ReactNative::ReactNativeIsland &island) noexcept {
  m_wkRootView = island;
}

winrt::Microsoft::ReactNative::ReactNativeIsland RootComponentView::ReactNativeIsland() noexcept {
  return m_wkRootView.get();
}

winrt::Microsoft::ReactNative::Composition::PortalComponentView RootComponentView::Portal() const noexcept {
  return m_wkPortal.get();
}

facebook::react::Point RootComponentView::getClientOffset() const noexcept {
  return {};
}

winrt::IUnknown RootComponentView::UiaProviderFromPoint(const POINT &ptPixels, const POINT &ptScreen) noexcept {
  facebook::react::Point ptDips{
      static_cast<facebook::react::Float>(ptPixels.x) / m_layoutMetrics.pointScaleFactor,
      static_cast<facebook::react::Float>(ptPixels.y) / m_layoutMetrics.pointScaleFactor};

  facebook::react::Point localPt;

  // In the case of a sub rootview, we may have a non-zero origin.  hitTest takes a pt in the parent coords, so we need
  // to apply the current origin
  ptDips += m_layoutMetrics.frame.origin;
  auto tag = hitTest(ptDips, localPt, true);

  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return nullptr;

  auto view = uiManager->GetViewRegistry().findComponentViewWithTag(tag);
  if (view == nullptr)
    return nullptr;

  auto uiaProvider = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view)->EnsureUiaProvider();

  // HACKHACK: It's ugly to have the RootComponentView know about the CompositionDynamicAutomationProvider type.
  // HACKHACK: Can we clean this up?
  auto dynamicProvider = uiaProvider.try_as<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>();
  if (dynamicProvider) {
    if (auto childProvider = dynamicProvider->TryGetChildSiteLinkAutomationProvider()) {
      // childProvider is the the automation provider from the ChildSiteLink.  In the case of WinUI, this
      // is a pointer to WinUI's internal CUIAHostWindow object.
      // It seems odd, but even though this node doesn't behave as a fragment root in our case (the real fragment root
      // is the RootComponentView's UIA provider), we still use its IRawElementProviderFragmentRoot -- just so
      // we can do the ElementProviderFromPoint call.  (this was recommended by the team who did the initial
      // architecture work).
      if (auto fragmentRoot = childProvider.try_as<IRawElementProviderFragmentRoot>())
      {
        com_ptr<IRawElementProviderFragment> frag;
        fragmentRoot->ElementProviderFromPoint(
            ptScreen.x,   // Note since we're going through IRawElementProviderFragment the coordinates are in screen space.
            ptScreen.y,
            frag.put());
        // In the case of WinUI, frag is now the UIA provider for the specific WinUI element that was hit.
        // (A Microsoft_UI_Xaml!CUIAWrapper object)
        if (frag)
        {
          return frag.as<winrt::IUnknown>();
        }
      }
    }
  }

  return uiaProvider;
}

float RootComponentView::FontSizeMultiplier() const noexcept {
  if (auto rootView = m_wkRootView.get()) {
    return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)
        ->FontSizeMultiplier();
  }
  assert(false);
  return 1.0f;
}

winrt::Windows::Foundation::Point RootComponentView::ConvertScreenToLocal(
    winrt::Windows::Foundation::Point pt) noexcept {
  if (auto rootView = m_wkRootView.get()) {
    return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)
        ->ConvertScreenToLocal(pt);
  }
  assert(false);
  return {};
}

winrt::Windows::Foundation::Point RootComponentView::ConvertLocalToScreen(
    winrt::Windows::Foundation::Point pt) noexcept {
  if (auto rootView = m_wkRootView.get()) {
    return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)
        ->ConvertLocalToScreen(pt);
  }
  assert(false);
  return {};
}

winrt::Microsoft::UI::Content::ContentIsland RootComponentView::parentContentIsland() noexcept {
  if (auto rootView = m_wkRootView.get()) {
    return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->Island();
  }
  return nullptr;
}

winrt::Microsoft::ReactNative::implementation::ClipState RootComponentView::getClipState() noexcept {
  return winrt::Microsoft::ReactNative::implementation::ClipState::NoClip;
}

HWND RootComponentView::GetHwndForParenting() noexcept {
  if (auto rootView = m_wkRootView.get()) {
    auto hwnd = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)
                    ->GetHwndForParenting();
    if (hwnd)
      return hwnd;
  }

  return base_type::GetHwndForParenting();
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
