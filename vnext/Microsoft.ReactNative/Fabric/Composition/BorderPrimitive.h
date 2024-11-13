
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "CompositionHelpers.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ComponentView;

// Controls adding/removing appropriate visuals to a parent to render a specific border without requiring
struct BorderPrimitive {
  static constexpr size_t SpecialBorderLayerCount = 8;

  // Use when attaching to an existing visual - We do this for View's border properties, to optimize the number of
  // visuals for each view.
  BorderPrimitive(
      winrt::Microsoft::ReactNative::Composition::implementation::ComponentView &outer,
      const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &rootVisual);

  // This ctor creates a new root visual that can then be added to the visual tree
  BorderPrimitive(winrt::Microsoft::ReactNative::Composition::implementation::ComponentView &outer);

  void markNeedsUpdate() noexcept;

  // We hoist focus visuals up the tree to allow them to be higher in the z-order.
  // This means a single BorderPrimitive may change the owning ComponentView as focus moves around
  void setOuter(winrt::Microsoft::ReactNative::Composition::implementation::ComponentView *outer) noexcept;

  void updateProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps) noexcept;
  void finalize(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::BorderMetrics &borderMetrics) noexcept;
  void onThemeChanged(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::BorderMetrics &borderMetrics) noexcept;

  uint8_t numberOfVisuals() const noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual RootVisual() const noexcept;

  static facebook::react::BorderMetrics resolveAndAlignBorderMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;

  static bool requiresBorder(
      const facebook::react::BorderMetrics &borderMetrics,
      winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme) noexcept;

  static winrt::com_ptr<ID2D1PathGeometry> GenerateRoundedRectPathGeometry(
      winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      const facebook::react::RectangleCorners<float> &baseRadius,
      const facebook::react::RectangleEdges<float> &inset,
      const facebook::react::RectangleEdges<float> &rectPathGeometry) noexcept;

 private:
  std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount>
  FindSpecialBorderLayers() const noexcept;

  bool TryUpdateSpecialBorderLayers(
      winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme,
      std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount>
          &spBorderVisuals,
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::BorderMetrics &borderMetrics) noexcept;

  uint8_t m_numBorderVisuals{0};
  winrt::Microsoft::ReactNative::Composition::implementation::ComponentView *m_outer;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_rootVisual{nullptr};
  bool m_needsUpdate{true};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
