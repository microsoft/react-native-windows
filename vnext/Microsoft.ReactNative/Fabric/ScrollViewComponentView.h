
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include "ViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/scrollview/ScrollViewProps.h>
#pragma warning(pop)

namespace Microsoft::ReactNative {

struct ScrollViewComponentView : BaseComponentView {
  ScrollViewComponentView();

  std::vector<facebook::react::ComponentDescriptorProvider>
  supplementalComponentDescriptorProviders() noexcept override;
  void mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void updateProps(
      facebook::react::Props::Shared const &props,
      facebook::react::Props::Shared const &oldProps) noexcept override;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  void updateState(
      facebook::react::State::Shared const &state,
      facebook::react::State::Shared const &oldState) noexcept override;
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

  winrt::Microsoft::ReactNative::ViewPanel m_contentPanel;
};

} // namespace Microsoft::ReactNative
