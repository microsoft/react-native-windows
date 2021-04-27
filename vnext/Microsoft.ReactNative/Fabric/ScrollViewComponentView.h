
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include "ViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewProps.h>
#pragma warning(pop)

namespace Microsoft::ReactNative {

struct ScrollViewComponentView : BaseComponentView {
  ScrollViewComponentView();

  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  void mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedProps props() noexcept override;

  const xaml::FrameworkElement Element() const noexcept override;

 private:
  facebook::react::LayoutMetrics m_layoutMetrics;
  xaml::Controls::ScrollViewer m_element;

  xaml::FrameworkElement::SizeChanged_revoker m_scrollViewerSizeChangedRevoker{};
  xaml::FrameworkElement::SizeChanged_revoker m_contentSizeChangedRevoker{};
  xaml::Controls::ScrollViewer::ViewChanged_revoker m_scrollViewerViewChangedRevoker{};
  xaml::Controls::ScrollViewer::ViewChanging_revoker m_scrollViewerViewChangingRevoker{};
  xaml::Controls::ScrollViewer::DirectManipulationCompleted_revoker m_scrollViewerDirectManipulationCompletedRevoker{};
  xaml::Controls::ScrollViewer::DirectManipulationStarted_revoker m_scrollViewerDirectManipulationStartedRevoker{};
  xaml::Controls::Control::Loaded_revoker m_controlLoadedRevoker{};

  float m_zoomFactor{1.0f};
  bool m_isScrollingFromInertia = false;
  bool m_isScrolling = false;
  bool m_isHorizontal = false;
  bool m_isScrollingEnabled = true;
  bool m_changeViewAfterLoaded = false;
  bool m_dismissKeyboardOnDrag = false;

  winrt::Microsoft::ReactNative::ViewPanel m_contentPanel;
};

} // namespace Microsoft::ReactNative
