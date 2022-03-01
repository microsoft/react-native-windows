
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include "CompViewComponentView.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewProps.h>
#pragma warning(pop)
#include <winrt/Windows.UI.Composition.interactions.h>

namespace Microsoft::ReactNative {

struct CompScrollViewComponentView;

struct CompScrollViewComponentView : CompBaseComponentView {
  struct ScrollInteractionTrackerOwner : public winrt::implements<
                                             ScrollInteractionTrackerOwner,
                                             winrt::Windows::UI::Composition::Interactions::IInteractionTrackerOwner> {
    ScrollInteractionTrackerOwner(CompScrollViewComponentView *outer);

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
    CompScrollViewComponentView *m_outer;
  };

  CompScrollViewComponentView();

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
  facebook::react::SharedProps props() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  const winrt::Windows::UI::Composition::Visual Visual() const noexcept override;

  const winrt::Windows::UI::Composition::ContainerVisual ContentVisual() const noexcept;
  // void OnPointerDown(const winrt::Windows::UI::Input::PointerPoint &pp) noexcept override;
  bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept override;

 private:
  void ensureVisual() noexcept;
  void updateInteractionMaxPosition() noexcept;

  facebook::react::LayoutMetrics m_layoutMetrics;
  winrt::Windows::UI::Composition::Visual m_visual{nullptr};
  winrt::Windows::UI::Composition::ContainerVisual m_contentVisual{nullptr};
  winrt::Windows::UI::Composition::Interactions::InteractionTracker m_interactionTracker{nullptr};
  winrt::Windows::UI::Composition::Interactions::VisualInteractionSource m_visualInteractionSource{nullptr};

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
