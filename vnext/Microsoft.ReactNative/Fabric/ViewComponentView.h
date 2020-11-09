
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#include <react/renderer/components/view/ViewProps.h>
#pragma warning(pop)

#include "ComponentView.h"

namespace Microsoft::ReactNative {

struct BaseComponentView : IComponentView {
  virtual const xaml::FrameworkElement Element() const noexcept = 0;
};

struct ViewComponentView : BaseComponentView {
  ViewComponentView();

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

  virtual const xaml::FrameworkElement Element() const noexcept;

 private:
  bool m_needsBorderUpdate{false};
  facebook::react::SharedViewProps m_props;
  facebook::react::LayoutMetrics m_layoutMetrics;
  winrt::Microsoft::ReactNative::ViewPanel m_element;
};

} // namespace Microsoft::ReactNative
