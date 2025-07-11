
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>

#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionHelpers.h"
#include "CompositionViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/view/ViewProps.h>
#pragma warning(pop)
#include "Composition.ContentIslandComponentView.g.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ContentIslandComponentView : ContentIslandComponentViewT<ContentIslandComponentView, ViewComponentView> {
  using Super = ContentIslandComponentViewT<ContentIslandComponentView, ViewComponentView>;

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void Connect(const winrt::Microsoft::UI::Content::ContentIsland &contentIsland) noexcept;

  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept;

  void prepareForRecycle() noexcept override;

  bool focusable() const noexcept override;

  winrt::Windows::Foundation::IInspectable EnsureUiaProvider() noexcept override;

  void onGotFocus(const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept override;

  ContentIslandComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      ReactCompositionViewComponentBuilder *builder);
  ~ContentIslandComponentView() noexcept;

 private:
  void OnMounted() noexcept;
  void OnUnmounted() noexcept;
  void ParentLayoutChanged() noexcept;

  bool m_layoutChangePosted{false};
  winrt::Microsoft::UI::Content::ContentIsland m_islandToConnect{nullptr};
  winrt::event_token m_mountedToken;
  winrt::event_token m_unmountedToken;
  std::vector<winrt::Microsoft::ReactNative::ComponentView::LayoutMetricsChanged_revoker> m_layoutMetricChangedRevokers;
  //winrt::Microsoft::UI::Content::ChildSiteLink m_childSiteLink{nullptr};
  winrt::Microsoft::UI::Content::DesktopChildSiteBridge m_childSiteBridge{nullptr};
  winrt::Microsoft::UI::Input::InputFocusNavigationHost m_navigationHost{nullptr};
  winrt::event_token m_navigationHostDepartFocusRequestedToken{};

  // Automation
  void ConfigureChildSiteLinkAutomation() noexcept;
  winrt::event_token m_fragmentRootAutomationProviderRequestedToken{};
  winrt::event_token m_parentAutomationProviderRequestedToken{};
  winrt::event_token m_nextSiblingAutomationProviderRequestedToken{};
  winrt::event_token m_previousSiblingAutomationProviderRequestedToken{};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
