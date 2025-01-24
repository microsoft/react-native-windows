
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ContentIslandComponentView.h"

#include <Fabric/FabricUIManagerModule.h>
#include <IReactContext.h>
#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionContextHelper.h"
#include "RootComponentView.h"

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

void ContentIslandComponentView::OnMounted() noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
  m_childSiteLink = winrt::Microsoft::UI::Content::ChildSiteLink::Create(
      rootComponentView()->parentContentIsland(),
      winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(Visual())
          .as<winrt::Microsoft::UI::Composition::ContainerVisual>());
  m_childSiteLink.ActualSize({m_layoutMetrics.frame.size.width, m_layoutMetrics.frame.size.height});

  m_navigationHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteLink(m_childSiteLink);

  m_departFocusRequestedRevoker =
      m_navigationHost.DepartFocusRequested(winrt::auto_revoke, [wkThis = get_weak()](const auto &, const auto &args) {
        if (auto strongThis = wkThis.get()) {
          const bool next = (args.Request().Reason() == winrt::Microsoft::UI::Input::FocusNavigationReason::First);
          strongThis->rootComponentView()->TryMoveFocus(next);
          args.Result(winrt::Microsoft::UI::Input::FocusNavigationResult::Moved);
        }
      });

  // This automation mode must be set before connecting the child content island.
  // It puts the child content into a mode where it won't own its own framework root.  Instead, the child island's
  // automation peers will use the same framework root as the automation peer of this ContentIslandComponentView.
  m_childSiteLink.AutomationTreeOption(winrt::Microsoft::UI::Content::AutomationTreeOptions::FragmentBased);

  if (m_islandToConnect) {
    m_childSiteLink.Connect(m_islandToConnect);
    m_islandToConnect = nullptr;
  }

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
    view = view.Parent();
  }
#endif
}

void ContentIslandComponentView::OnUnmounted() noexcept {
  m_layoutMetricChangedRevokers.clear();
#ifdef USE_EXPERIMENTAL_WINUI3
  m_departFocusRequestedRevoker.revoke();
#endif
}

void ContentIslandComponentView::ParentLayoutChanged() noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
  if (m_layoutChangePosted)
    return;

  m_layoutChangePosted = true;
  ReactContext().UIDispatcher().Post([wkThis = get_weak()]() {
    if (auto strongThis = wkThis.get()) {
      auto clientRect = strongThis->getClientRect();

      strongThis->m_childSiteLink.LocalToParentTransformMatrix(
          winrt::Windows::Foundation::Numerics::make_float4x4_translation(
              static_cast<float>(clientRect.left), static_cast<float>(clientRect.top), 0.0f));

      strongThis->m_layoutChangePosted = false;
    }
  });
#endif
}

winrt::IInspectable ContentIslandComponentView::EnsureUiaProvider() noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
  if (m_uiaProvider == nullptr) {
    m_uiaProvider = winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>(
        *get_strong(), m_childSiteLink);
  }
  return m_uiaProvider;
#else
  return Super::EnsureUiaProvider();
#endif
}

bool ContentIslandComponentView::focusable() const noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
  // We don't have a way to check to see if the ContentIsland has focusable content,
  // so we'll always return true.  We'll have to handle the case where the content doesn't have
  // focusable content in the OnGotFocus handler.
  return true;
#else
  return Super::focusable();
#endif
}

void ContentIslandComponentView::onGotFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
  (void)args;
  // TODO: Need to make sure we handle shift-tab as well (which should use "Last" instead of "First")
  m_navigationHost.NavigateFocus(winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
      winrt::Microsoft::UI::Input::FocusNavigationReason::First));
#else
  return Super::onGotFocus(args);
#endif
}

ContentIslandComponentView::~ContentIslandComponentView() noexcept {
  if (m_islandToConnect) {
    m_islandToConnect.Close();
  }
}

void ContentIslandComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void ContentIslandComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

void ContentIslandComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
  if (m_childSiteLink) {
    m_childSiteLink.ActualSize({layoutMetrics.frame.size.width, layoutMetrics.frame.size.height});
    ParentLayoutChanged();
  }
#endif
  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

void ContentIslandComponentView::Connect(const winrt::Microsoft::UI::Content::ContentIsland &contentIsland) noexcept {
#ifdef USE_EXPERIMENTAL_WINUI3
  if (m_childSiteLink) {
    m_islandToConnect = nullptr;
    m_childSiteLink.Connect(contentIsland);
  } else {
    m_islandToConnect = contentIsland;
  }
#endif
}

void ContentIslandComponentView::prepareForRecycle() noexcept {
  Super::prepareForRecycle();
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
