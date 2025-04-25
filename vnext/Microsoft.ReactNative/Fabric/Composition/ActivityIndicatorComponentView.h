// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "CompositionViewComponentView.h"

#include "Composition.ActivityIndicatorComponentView.g.h"
#include <react/components/rnwcore/ShadowNodes.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ActivityIndicatorComponentView
    : ActivityIndicatorComponentViewT<ActivityIndicatorComponentView, ViewComponentView> {
  using Super = ActivityIndicatorComponentViewT<ActivityIndicatorComponentView, ViewComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept override;
  void onThemeChanged() noexcept override;

  virtual std::string DefaultControlType() const noexcept;

  ActivityIndicatorComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  static facebook::react::SharedViewProps defaultProps() noexcept;
  const facebook::react::ActivityIndicatorViewProps &activityIndicatorViewProps() const noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual createVisual() noexcept override;

 private:
  void updateVisualSize() noexcept;
  void updateProgressColor(const facebook::react::SharedColor &color) noexcept;

  bool m_animationStopped{false};

  winrt::Microsoft::ReactNative::Composition::Experimental::IActivityVisual m_ActivityIndicatorVisual{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
