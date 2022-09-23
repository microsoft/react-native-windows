
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>

#include "CompositionViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewProps.h>
#pragma warning(pop)
#include <winrt/Windows.UI.Composition.interactions.h>

namespace Microsoft::ReactNative {

struct ScrollViewComponentView;

struct ScrollViewComponentView : CompositionBaseComponentView {
  /*
struct ScrollInteractionTrackerOwner : public winrt::implements<
                                           ScrollInteractionTrackerOwner,
                                           winrt::Windows::UI::Composition::Interactions::IInteractionTrackerOwner> {
  ScrollInteractionTrackerOwner(ScrollViewComponentView *outer);

  void CustomAnimationStateEntered(
      winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs args) noexcept;
  void IdleStateEntered(
      winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs args) noexcept;
  void InertiaStateEntered(
      winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs args) noexcept;
  void InteractingStateEntered(
      winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs args) noexcept;
  void RequestIgnored(
      winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs args) noexcept;
  void ValuesChanged(
      winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs args) noexcept;

 private:
  ScrollViewComponentView *m_outer;
};

*/

  using Super = CompositionBaseComponentView;
  ScrollViewComponentView(
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

  // void OnPointerDown(const winrt::Windows::UI::Input::PointerPoint &pp) noexcept override;
  bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept override;

 private:
  void ensureVisual() noexcept;
  void updateContentVisualSize() noexcept;

  facebook::react::Size m_contentSize;
  winrt::Microsoft::ReactNative::Composition::ScrollVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::ScrollVisual::ScrollPositionChanged_revoker
      m_scrollPositionChangedRevoker{};

  facebook::react::SharedViewProps m_props;
  float m_zoomFactor{1.0f};
  bool m_isScrollingFromInertia = false;
  bool m_isScrolling = false;
  bool m_isHorizontal = false;
  bool m_isScrollingEnabled = true;
  bool m_changeViewAfterLoaded = false;
  bool m_dismissKeyboardOnDrag = false;

 private:
  bool shouldBeControl() const noexcept;
};

} // namespace Microsoft::ReactNative
