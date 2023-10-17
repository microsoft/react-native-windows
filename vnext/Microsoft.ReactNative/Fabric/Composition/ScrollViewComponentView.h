
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

  [[nodiscard]] static std::shared_ptr<ScrollViewComponentView> Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag) noexcept;

  void mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
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
  void onKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;

  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

  void onPointerWheelChanged(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;

  void StartBringIntoView(BringIntoViewOptions &&args) noexcept override;
  virtual std::string DefaultControlType() const noexcept;

 private:
  ScrollViewComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);

  void ensureVisual() noexcept;
  void updateContentVisualSize() noexcept;
  bool scrollToEnd(bool animate) noexcept;
  bool scrollToStart(bool animate) noexcept;
  bool pageUp(bool animate) noexcept;
  bool pageDown(bool animate) noexcept;
  bool lineUp(bool animate) noexcept;
  bool lineDown(bool animate) noexcept;
  bool lineLeft(bool animate) noexcept;
  bool lineRight(bool animate) noexcept;
  bool scrollDown(float delta, bool animate) noexcept;
  bool scrollUp(float delta, bool animate) noexcept;
  bool scrollLeft(float delta, bool aniamte) noexcept;
  bool scrollRight(float delta, bool animate) noexcept;

  facebook::react::Size m_contentSize;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::IScrollVisual m_scrollVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::IScrollVisual::ScrollPositionChanged_revoker
      m_scrollPositionChangedRevoker{};

  facebook::react::SharedViewProps m_props;
  float m_zoomFactor{1.0f};
  bool m_isScrollingFromInertia = false;
  bool m_isScrolling = false;
  bool m_isHorizontal = false;
  bool m_changeViewAfterLoaded = false;
  bool m_dismissKeyboardOnDrag = false;

 private:
  bool shouldBeControl() const noexcept;
};

} // namespace Microsoft::ReactNative
