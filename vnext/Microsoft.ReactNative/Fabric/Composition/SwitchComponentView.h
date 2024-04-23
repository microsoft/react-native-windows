
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>

#include "Composition.SwitchComponentView.g.h"
#include "CompositionViewComponentView.h"

#include <react/components/rnwcore/ShadowNodes.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct SwitchComponentView : SwitchComponentViewT<SwitchComponentView, ComponentView> {
  using Super = SwitchComponentViewT<SwitchComponentView, ComponentView>;

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
  void HandleCommand(winrt::hstring commandName, const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept
      override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedViewProps viewProps() noexcept override;
  bool focusable() const noexcept override;
  void onThemeChanged() noexcept override;
  void OnKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void OnPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerEntered(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerExited(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual Visual() const noexcept override;
  std::string DefaultControlType() const noexcept override;

  SwitchComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  void ensureVisual() noexcept;
  bool toggle() noexcept;
  void handleScaleChange() noexcept;
  void updateVisuals() noexcept;

  bool m_hovered{false};
  bool m_pressed{false};
  bool m_supressAnimationForNextFrame{false};
  bool m_visualUpdateRequired{true};
  facebook::react::Size m_contentSize;
  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual m_trackVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual m_thumbVisual{nullptr};
  facebook::react::SharedViewProps m_props;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
