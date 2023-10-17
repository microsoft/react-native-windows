
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#include "CompositionViewComponentView.h"

#include <react/components/rnwcore/ShadowNodes.h>

namespace Microsoft::ReactNative {

struct SwitchComponentView;

struct SwitchComponentView : CompositionBaseComponentView {
  using Super = CompositionBaseComponentView;

  [[nodiscard]] static std::shared_ptr<SwitchComponentView> Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::Props::Shared props() noexcept override;
  bool focusable() const noexcept override;
  void onKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void onPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;
  std::string DefaultControlType() const noexcept override;

 private:
  SwitchComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  void ensureVisual() noexcept;
  void Draw() noexcept;
  void ensureDrawingSurface() noexcept;
  bool toggle() noexcept;

  facebook::react::Size m_contentSize;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  facebook::react::SharedViewProps m_props;
  winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush m_drawingSurface;
  winrt::Microsoft::ReactNative::Composition::ISwitchThumbVisual m_thumbVisual;
};

} // namespace Microsoft::ReactNative
