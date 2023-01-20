
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>

#include "CompositionViewComponentView.h"

//#pragma warning(push)
//#pragma warning(disable : 4305)
//#include <react/renderer/components/scrollview/ScrollViewProps.h>
//#pragma warning(pop)
//#include <winrt/Windows.UI.Composition.interactions.h>

namespace Microsoft::ReactNative {

struct SwitchComponentView;

struct SwitchComponentView : CompositionBaseComponentView {

  using Super = CompositionBaseComponentView;
  SwitchComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);

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
  facebook::react::Props::Shared props() noexcept override;
  facebook::react::SharedTouchEventEmitter touchEventEmitter() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

 private:
  void ensureVisual() noexcept;
  void updateContentVisualSize() noexcept;

  facebook::react::Size m_contentSize;
  winrt::Microsoft::ReactNative::Composition::SpriteVisual m_visual{nullptr};

  facebook::react::SharedViewProps m_props;
  //float m_zoomFactor{1.0f};
  //bool m_isScrollingFromInertia = false;
  //bool m_isScrolling = false;
  //bool m_isHorizontal = false;
  //bool m_isScrollingEnabled = true;
  //bool m_changeViewAfterLoaded = false;
  //bool m_dismissKeyboardOnDrag = false;

 private:
  //bool shouldBeControl() const noexcept;
};

} // namespace Microsoft::ReactNative
