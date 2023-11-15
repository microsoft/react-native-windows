
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "../CompositionViewComponentView.h"

#include <react/components/rnwcore/ShadowNodes.h>

namespace Microsoft::ReactNative {

struct WindowsModalHostComponentView : CompositionBaseComponentView {
  using Super = CompositionBaseComponentView;

  [[nodiscard]] static std::shared_ptr<WindowsModalHostComponentView> Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;

  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;

  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedViewProps viewProps() noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual OuterVisual() const noexcept override;

 private:
  WindowsModalHostComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);
  std::shared_ptr<facebook::react::ModalHostViewProps const> m_props;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
};

} // namespace Microsoft::ReactNative
