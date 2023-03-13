
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include "CompositionViewComponentView.h"
#include <Fabric/AbiViewProps.h>

#include <react/components/rnwcore/ShadowNodes.h>

namespace Microsoft::ReactNative {

struct AbiCompositionViewComponentView : CompositionBaseComponentView {
  using Super = CompositionBaseComponentView;
  AbiCompositionViewComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::Composition::ICompositionViewComponent userComponent);

  void mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;

  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;

  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  bool focusable() const noexcept override;
  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  facebook::react::Props::Shared props() noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

 private:
  winrt::Microsoft::ReactNative::Composition::ICompositionViewComponent m_userComponent;
  std::shared_ptr<AbiViewProps const> m_props;
  winrt::Microsoft::ReactNative::Composition::IVisual m_visual{nullptr};
};

} // namespace Microsoft::ReactNative
