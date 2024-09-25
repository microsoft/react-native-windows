
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "Composition.UnimplementedNativeViewComponentView.g.h"
#include "CompositionViewComponentView.h"

#include <react/components/rnwcore/ShadowNodes.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct UnimplementedNativeViewComponentView
    : public UnimplementedNativeViewComponentViewT<UnimplementedNativeViewComponentView, ViewComponentView> {
  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;

  void HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept override;

  UnimplementedNativeViewComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  std::shared_ptr<facebook::react::UnimplementedNativeViewProps const> m_props;
  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_labelVisual{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
