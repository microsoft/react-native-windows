
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/text/TextProps.h>
#include "ComponentView.h"
#include "ViewComponentView.h"

namespace Microsoft::ReactNative {

struct TextComponentView : BaseComponentView {
  TextComponentView();

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

  virtual const xaml::FrameworkElement Element() const noexcept override;

 private:
  facebook::react::LayoutMetrics m_layoutMetrics;
  xaml::Controls::TextBlock m_element;
};

} // namespace Microsoft::ReactNative
