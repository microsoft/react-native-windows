
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
  m_childSiteLink = winrt::Microsoft::UI::Content::ChildSiteLink::Create(
      rootComponentView()->parentContentIsland(),
      winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(Visual())
          .as<winrt::Microsoft::UI::Composition::ContainerVisual>());
  m_childSiteLink.ActualSize({m_layoutMetrics.frame.size.width, m_layoutMetrics.frame.size.height});

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
}

void ContentIslandComponentView::OnUnmounted() noexcept {
  m_layoutMetricChangedRevokers.clear();
  if (m_navigationHostDepartFocusRequestedToken && m_navigationHost) {
    m_navigationHost.DepartFocusRequested(m_navigationHostDepartFocusRequestedToken);
    m_navigationHostDepartFocusRequestedToken = {};
  }
}

void ContentIslandComponentView::ParentLayoutChanged() noexcept {
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
}

winrt::IInspectable ContentIslandComponentView::EnsureUiaProvider() noexcept {
  if (m_uiaProvider == nullptr) {
    m_uiaProvider = winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>(
        *get_strong(), m_childSiteLink);
  }
  return m_uiaProvider;
}

bool ContentIslandComponentView::focusable() const noexcept {
  // We don't have a way to check to see if the ContentIsland has focusable content,
  // so we'll always return true.  We'll have to handle the case where the content doesn't have
  // focusable content in the OnGotFocus handler.
  return true;
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
  if (m_childSiteLink) {
    if (m_fragmentRootAutomationProviderRequestedToken) {
      m_childSiteLink.FragmentRootAutomationProviderRequested(m_fragmentRootAutomationProviderRequestedToken);
      m_fragmentRootAutomationProviderRequestedToken = {};
    }
    if (m_parentAutomationProviderRequestedToken) {
      m_childSiteLink.ParentAutomationProviderRequested(m_parentAutomationProviderRequestedToken);
      m_parentAutomationProviderRequestedToken = {};
    }
    if (m_nextSiblingAutomationProviderRequestedToken) {
      m_childSiteLink.NextSiblingAutomationProviderRequested(m_nextSiblingAutomationProviderRequestedToken);
      m_nextSiblingAutomationProviderRequestedToken = {};
    }
    if (m_previousSiblingAutomationProviderRequestedToken) {
      m_childSiteLink.PreviousSiblingAutomationProviderRequested(m_previousSiblingAutomationProviderRequestedToken);
      m_previousSiblingAutomationProviderRequestedToken = {};
    }
  }
  if (m_islandToConnect) {
    m_islandToConnect.Close();
  }
}

void ContentIslandComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  // I assume this is here b/c it doesn't make sense for a Island CV to have children.
  // XamlHost does have children, but the children will represent Xaml elements and
  // don't participate direction in RN's rendering.
  // assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void ContentIslandComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  // assert(false);
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
  // This automation mode must be set before connecting the child ContentIsland.
  // It puts the child content into a mode where it won't own its own framework root.  Instead, the child island's
  // automation peers will use the same framework root as the automation peer of this ContentIslandComponentView.
  m_childSiteLink.AutomationOption(winrt::Microsoft::UI::Content::ContentAutomationOptions::FragmentBased);

  // These events are raised in response to the child ContentIsland asking for providers.
  // For example, the ContentIsland.FragmentRootAutomationProvider property will return
  // the provider we provide here in FragmentRootAutomationProviderRequested.

  // We capture "this" as a raw pointer because ContentIslandComponentView doesn't currently support weak ptrs.
  // It's safe because we disconnect these events in the destructor.

  m_fragmentRootAutomationProviderRequestedToken = m_childSiteLink.FragmentRootAutomationProviderRequested(
      [this](
          const winrt::Microsoft::UI::Content::IContentSiteAutomation &,
          const winrt::Microsoft::UI::Content::ContentSiteAutomationProviderRequestedEventArgs &args) {
        // The child island's fragment tree doesn't have its own fragment root.
        // Here's how we can provide the correct fragment root to the child's UIA logic.
        winrt::com_ptr<IRawElementProviderFragmentRoot> fragmentRoot{nullptr};
        auto uiaProvider = this->EnsureUiaProvider();
        uiaProvider.as<IRawElementProviderFragment>()->get_FragmentRoot(fragmentRoot.put());
        args.AutomationProvider(fragmentRoot.as<IInspectable>());
        args.Handled(true);
      });

  m_parentAutomationProviderRequestedToken = m_childSiteLink.ParentAutomationProviderRequested(
      [this](
          const winrt::Microsoft::UI::Content::IContentSiteAutomation &,
          const winrt::Microsoft::UI::Content::ContentSiteAutomationProviderRequestedEventArgs &args) {
        auto uiaProvider = this->EnsureUiaProvider();
        args.AutomationProvider(uiaProvider);
        args.Handled(true);
      });

  m_nextSiblingAutomationProviderRequestedToken = m_childSiteLink.NextSiblingAutomationProviderRequested(
      [](const winrt::Microsoft::UI::Content::IContentSiteAutomation &,
         const winrt::Microsoft::UI::Content::ContentSiteAutomationProviderRequestedEventArgs &args) {
        // The ContentIsland will always be the one and only child of this node, so it won't have siblings.
        args.AutomationProvider(nullptr);
        args.Handled(true);
      });

  m_previousSiblingAutomationProviderRequestedToken = m_childSiteLink.PreviousSiblingAutomationProviderRequested(
      [](const winrt::Microsoft::UI::Content::IContentSiteAutomation &,
         const winrt::Microsoft::UI::Content::ContentSiteAutomationProviderRequestedEventArgs &args) {
        // The ContentIsland will always be the one and only child of this node, so it won't have siblings.
        args.AutomationProvider(nullptr);
        args.Handled(true);
      });
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
