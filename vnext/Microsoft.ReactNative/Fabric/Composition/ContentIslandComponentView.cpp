
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ContentIslandComponentView.h"

#include <Fabric/FabricUIManagerModule.h>
#include <IReactContext.h>
#include <Utils/ValueUtils.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionContextHelper.h"
#include "RootComponentView.h"
#include "ScrollViewComponentView.h"

#include "Composition.ContentIslandComponentView.g.cpp"

#include "CompositionDynamicAutomationProvider.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ContentIslandComponentView::ContentIslandComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    ReactCompositionViewComponentBuilder *builder)
    : base_type(
          ViewComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default,
          builder) {
  m_mountedToken = Mounted([](const winrt::IInspectable &, const winrt::Microsoft::ReactNative::ComponentView &view) {
    view.as<ContentIslandComponentView>()->OnMounted();
  });
  m_unmountedToken =
      Unmounted([](const winrt::IInspectable &, const winrt::Microsoft::ReactNative::ComponentView &view) {
        view.as<ContentIslandComponentView>()->OnUnmounted();
      });
}

winrt::Microsoft::UI::Content::ContentIsland ContentIslandComponentView::ParentContentIsland() noexcept {
  auto root = rootComponentView();
  if (!root)
    return nullptr;
  return root->parentContentIsland();
}

void ContentIslandComponentView::ConnectInternal() noexcept {
  if (!m_islandToConnect)
    return;

  m_childSiteLink = winrt::Microsoft::UI::Content::ChildSiteLink::Create(
      m_parentContentIsland,
      winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(Visual())
          .as<winrt::Microsoft::UI::Composition::ContainerVisual>());
  m_childSiteLink.ActualSize({m_layoutMetrics.frame.size.width, m_layoutMetrics.frame.size.height});

  // Issue #15557: Set initial LocalToParentTransformMatrix synchronously before Connect.
  // This fixes popup position being wrong even without scrolling.
  // Note: getClientRect() returns physical pixels, but LocalToParentTransformMatrix expects DIPs.
  auto clientRect = getClientRect();
  float scaleFactor = m_layoutMetrics.pointScaleFactor;
  m_childSiteLink.LocalToParentTransformMatrix(
      winrt::Windows::Foundation::Numerics::make_float4x4_translation(
          static_cast<float>(clientRect.left) / scaleFactor, static_cast<float>(clientRect.top) / scaleFactor, 0.0f));

  m_navigationHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteLink(m_childSiteLink);

  m_navigationHostDepartFocusRequestedToken =
      m_navigationHost.DepartFocusRequested([wkThis = get_weak()](const auto &, const auto &args) {
        if (auto strongThis = wkThis.get()) {
          const bool next = (args.Request().Reason() != winrt::Microsoft::UI::Input::FocusNavigationReason::Last);
          strongThis->rootComponentView()->TryMoveFocus(next);
          args.Result(winrt::Microsoft::UI::Input::FocusNavigationResult::Moved);
        }
      });

  // We configure automation even if there's no UIA client at this point, because it's possible the first UIA
  // request we'll get will be for a child of this island calling upward in the UIA tree.
  ConfigureChildSiteLinkAutomation();

  if (m_islandToConnect) {
    m_childSiteLink.Connect(m_islandToConnect);
    m_islandToConnect = nullptr;
  }
  UnregisterForRootIslandEvents();

  ParentLayoutChanged();
  auto view = Parent();
  while (view) {
    m_layoutMetricChangedRevokers.push_back(view.LayoutMetricsChanged(
        winrt::auto_revoke,
        [wkThis = get_weak()](
            const winrt::IInspectable &sender, const winrt::Microsoft::ReactNative::LayoutMetricsChangedArgs &args) {
          if (auto strongThis = wkThis.get()) {
            strongThis->ParentLayoutChanged();
          }
        }));

    // Issue #15557: Register for ViewChanged on parent ScrollViews to update transform
    // when scroll position changes, ensuring correct XAML popup positioning.
    if (auto scrollView = view.try_as<winrt::Microsoft::ReactNative::Composition::ScrollViewComponentView>()) {
      auto token =
          scrollView.ViewChanged([wkThis = get_weak()](const winrt::IInspectable &, const winrt::IInspectable &) {
            if (auto strongThis = wkThis.get()) {
              strongThis->ParentLayoutChanged();
            }
          });
      m_viewChangedSubscriptions.push_back({scrollView, token});
    }

    view = view.Parent();
  }
}

void ContentIslandComponentView::RegisterForRootIslandEvents() noexcept {
  m_parentContentIsland = ParentContentIsland();

  if (m_parentContentIsland.IsConnected()) {
    ConnectInternal();
  } else {
    m_islandStateChangedToken = m_parentContentIsland.StateChanged(
        [wkThis = get_weak()](
            const winrt::Microsoft::UI::Content::ContentIsland & /*island*/,
            const winrt::Microsoft::UI::Content::ContentIslandStateChangedEventArgs & /*args*/) {
          if (auto strongThis = wkThis.get()) {
            strongThis->ConnectInternal();
          }
        });
  }
}

void ContentIslandComponentView::UnregisterForRootIslandEvents() noexcept {
  if (m_islandStateChangedToken) {
    m_parentContentIsland.StateChanged(m_islandStateChangedToken);
    m_islandStateChangedToken = {};
    m_parentContentIsland = nullptr;
  }
}

void ContentIslandComponentView::OnMounted() noexcept {
  RegisterForRootIslandEvents();
}

void ContentIslandComponentView::OnUnmounted() noexcept {
  m_layoutMetricChangedRevokers.clear();

  // Issue #15557: Unsubscribe from parent ScrollView events
  for (auto &subscription : m_viewChangedSubscriptions) {
    if (auto scrollView = subscription.scrollView.get()) {
      scrollView.ViewChanged(subscription.token);
    }
  }
  m_viewChangedSubscriptions.clear();

  if (m_navigationHostDepartFocusRequestedToken && m_navigationHost) {
    m_navigationHost.DepartFocusRequested(m_navigationHostDepartFocusRequestedToken);
    m_navigationHostDepartFocusRequestedToken = {};
  }
  UnregisterForRootIslandEvents();
}

void ContentIslandComponentView::ParentLayoutChanged() noexcept {
  // Issue #15557: Update transform synchronously to ensure correct popup position
  // when user clicks. Async updates via UIDispatcher().Post() were causing the
  // popup to open with stale transform values.
  //
  // Note: The original async approach was for batching notifications during layout passes.
  // However, LocalToParentTransformMatrix is a cheap call (just sets a matrix), and
  // synchronous updates are required to ensure correct popup position when clicked.
  //
  // getClientRect() returns values in physical pixels (scaled by pointScaleFactor),
  // but LocalToParentTransformMatrix expects logical pixels (DIPs). We need to divide
  // by the scale factor to convert.
  auto clientRect = getClientRect();
  float scaleFactor = m_layoutMetrics.pointScaleFactor;

  float x = static_cast<float>(clientRect.left) / scaleFactor;
  float y = static_cast<float>(clientRect.top) / scaleFactor;

  m_childSiteLink.LocalToParentTransformMatrix(
      winrt::Windows::Foundation::Numerics::make_float4x4_translation(x, y, 0.0f));
}

winrt::Windows::Foundation::IInspectable ContentIslandComponentView::CreateAutomationProvider() noexcept {
  m_innerAutomationProvider =
      winrt::make_self<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>(
          *get_strong(), m_childSiteLink);
  return *m_innerAutomationProvider;
}

bool ContentIslandComponentView::focusable() const noexcept {
  // We don't have a way to check to see if the ContentIsland has focusable content,
  // so we'll always return true.  We'll have to handle the case where the content doesn't have
  // focusable content in the OnGotFocus handler.
  return true;
}

facebook::react::Tag ContentIslandComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  // Check if the point is within the bounds of this ContentIslandComponentView.
  // This ensures that hit tests correctly return this view's tag for UIA purposes,
  // even when the actual content (XAML buttons, etc.) is hosted in the ContentIsland.
  auto props = viewProps();
  if ((ignorePointerEvents || props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }

  return -1;
}

// Helper to convert a FocusNavigationDirection to a FocusNavigationReason.
winrt::Microsoft::UI::Input::FocusNavigationReason GetFocusNavigationReason(
    winrt::Microsoft::ReactNative::FocusNavigationDirection direction) noexcept {
  switch (direction) {
    case winrt::Microsoft::ReactNative::FocusNavigationDirection::First:
    case winrt::Microsoft::ReactNative::FocusNavigationDirection::Next:
      return winrt::Microsoft::UI::Input::FocusNavigationReason::First;
    case winrt::Microsoft::ReactNative::FocusNavigationDirection::Last:
    case winrt::Microsoft::ReactNative::FocusNavigationDirection::Previous:
      return winrt::Microsoft::UI::Input::FocusNavigationReason::Last;
  }
  return winrt::Microsoft::UI::Input::FocusNavigationReason::Restore;
}

void ContentIslandComponentView::onGotFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  auto gotFocusEventArgs = args.as<winrt::Microsoft::ReactNative::implementation::GotFocusEventArgs>();
  const auto navigationReason = GetFocusNavigationReason(gotFocusEventArgs->Direction());
  m_navigationHost.NavigateFocus(winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(navigationReason));
}

ContentIslandComponentView::~ContentIslandComponentView() noexcept {
  if (m_navigationHostDepartFocusRequestedToken && m_navigationHost) {
    m_navigationHost.DepartFocusRequested(m_navigationHostDepartFocusRequestedToken);
    m_navigationHostDepartFocusRequestedToken = {};
  }
  if (m_islandToConnect) {
    m_islandToConnect.Close();
  }
}

void ContentIslandComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::MountChildComponentView(childComponentView, index);
}

void ContentIslandComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::UnmountChildComponentView(childComponentView, index);
}

void ContentIslandComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  if (m_childSiteLink) {
    m_childSiteLink.ActualSize({layoutMetrics.frame.size.width, layoutMetrics.frame.size.height});
    ParentLayoutChanged();
  }
  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

void ContentIslandComponentView::Connect(const winrt::Microsoft::UI::Content::ContentIsland &contentIsland) noexcept {
  if (m_childSiteLink) {
    m_islandToConnect = nullptr;
    m_childSiteLink.Connect(contentIsland);
  } else {
    m_islandToConnect = contentIsland;
  }
}

void ContentIslandComponentView::prepareForRecycle() noexcept {
  Super::prepareForRecycle();
}

void ContentIslandComponentView::ConfigureChildSiteLinkAutomation() noexcept {
  // Determine the automation option to use:
  // 1. If explicitly set via builder, use that
  // 2. Otherwise, default to FrameworkBased
  if (m_builder) {
    m_childSiteLink.AutomationOption(m_builder->ContentIslandChildSiteAutomationOption().value_or(
        winrt::Microsoft::UI::Content::ContentAutomationOptions::FrameworkBased));
  }

  if (m_innerAutomationProvider) {
    m_innerAutomationProvider->SetChildSiteLink(m_childSiteLink);
  }
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
