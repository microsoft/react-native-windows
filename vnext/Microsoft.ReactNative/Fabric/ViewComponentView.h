
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "ComponentView.h"

namespace Microsoft::ReactNative {

struct BaseComponentView : IComponentView {
  virtual const xaml::FrameworkElement Element() const noexcept = 0;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;

 protected:
  facebook::react::SharedViewEventEmitter m_eventEmitter;
};

struct ViewComponentView : BaseComponentView {
  ViewComponentView();

  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  void mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedProps props() noexcept override;

  virtual const xaml::FrameworkElement Element() const noexcept;

 private:
  bool shouldBeControl() const noexcept;

  bool m_needsBorderUpdate{false};
  bool m_needsControlUpdate{false};
  bool m_needsControl{false};
  facebook::react::SharedViewProps m_props;
  facebook::react::LayoutMetrics m_layoutMetrics;
  winrt::Microsoft::ReactNative::ViewControl m_control{nullptr};
  winrt::Microsoft::ReactNative::ViewPanel m_panel;
  xaml::Controls::Border m_outerBorder{nullptr};
};

} // namespace Microsoft::ReactNative
