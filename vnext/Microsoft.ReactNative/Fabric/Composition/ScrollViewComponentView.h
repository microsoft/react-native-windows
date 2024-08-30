
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>

#include "CompositionViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewProps.h>
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)
#include "Composition.ScrollViewComponentView.g.h"
#include <winrt/Windows.UI.Composition.interactions.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ScrollBarComponent;

struct ScrollViewComponentView : ScrollViewComponentViewT<ScrollViewComponentView, ViewComponentView> {
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
  using Super = ScrollViewComponentViewT<ScrollViewComponentView, ViewComponentView>;

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
  void prepareForRecycle() noexcept override;
  void OnKeyDown(const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;

  void HandleCommand(winrt::hstring commandName, const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept
      override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  facebook::react::Point getClientOffset() const noexcept override;

  void onThemeChanged() noexcept override;
  void OnPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerWheelChanged(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerCaptureLost() noexcept override;

  void StartBringIntoView(winrt::Microsoft::ReactNative::implementation::BringIntoViewOptions &&args) noexcept override;
  virtual std::string DefaultControlType() const noexcept;

  void scrollTo(winrt::Windows::Foundation::Numerics::float3 offset, bool animate) noexcept;

  static facebook::react::SharedViewProps defaultProps() noexcept;

  ScrollViewComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  bool pageUp(bool animate) noexcept;
  bool pageDown(bool animate) noexcept;
  bool lineUp(bool animate) noexcept;
  bool lineDown(bool animate) noexcept;
  bool lineLeft(bool animate) noexcept;
  bool lineRight(bool animate) noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual createVisual() noexcept override;

 private:
  void updateContentVisualSize() noexcept;
  bool scrollToEnd(bool animate) noexcept;
  bool scrollToStart(bool animate) noexcept;
  bool scrollDown(float delta, bool animate) noexcept;
  bool scrollUp(float delta, bool animate) noexcept;
  bool scrollLeft(float delta, bool aniamte) noexcept;
  bool scrollRight(float delta, bool animate) noexcept;
  void updateBackgroundColor(const facebook::react::SharedColor &color) noexcept;
  void updateStateWithContentOffset() noexcept;

  facebook::react::Size m_contentSize;
  winrt::Microsoft::ReactNative::Composition::Experimental::IScrollVisual m_scrollVisual{nullptr};
  std::shared_ptr<ScrollBarComponent> m_horizontalScrollbarComponent{nullptr};
  std::shared_ptr<ScrollBarComponent> m_verticalScrollbarComponent{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IScrollVisual::ScrollPositionChanged_revoker
      m_scrollPositionChangedRevoker{};

  float m_zoomFactor{1.0f};
  bool m_isScrollingFromInertia = false;
  bool m_isScrolling = false;
  bool m_isHorizontal = false;
  bool m_changeViewAfterLoaded = false;
  bool m_dismissKeyboardOnDrag = false;
  std::shared_ptr<facebook::react::ScrollViewShadowNode::ConcreteState const> m_state;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
