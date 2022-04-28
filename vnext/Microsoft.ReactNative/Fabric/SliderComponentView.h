
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <UI.Xaml.Controls.h>
#include <react/renderer/components/rnwcore/Props.h>
#include <react/renderer/core/LayoutConstraints.h>
#include "SliderShadowNode.h"
#include "ViewComponentView.h"
#include "YogaXamlPanel.h"

namespace Microsoft::ReactNative {

struct SliderComponentView : BaseComponentView {
  using Super = BaseComponentView;
  SliderComponentView(winrt::Microsoft::ReactNative::ReactContext const &reactContext);

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

  const xaml::FrameworkElement Element() const noexcept override;

 private:
  bool m_needsOnLoadStart{false};
  std::shared_ptr<facebook::react::SliderProps const> m_props;
  std::shared_ptr<facebook::react::SliderShadowNode::ConcreteState const> m_state;
  facebook::react::LayoutConstraints m_layoutConstraints;
  facebook::react::LayoutMetrics m_layoutMetrics;
  winrt::Microsoft::ReactNative::YogaXamlPanel m_element;
  xaml::Controls::Slider m_slider;
  xaml::Controls::Slider::ValueChanged_revoker m_valueChangedRevoker;
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
