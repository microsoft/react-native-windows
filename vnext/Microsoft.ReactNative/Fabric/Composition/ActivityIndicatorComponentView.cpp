// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ActivityIndicatorComponentView.h"
#include "CompositionDynamicAutomationProvider.h"

#include <Windows.UI.Composition.h>
#include <Windows.h>
#include "CompositionContextHelper.h"
#include "RootComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

winrt::Microsoft::ReactNative::ComponentView ActivityIndicatorComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ActivityIndicatorComponentView>(compContext, tag, reactContext);
}

ActivityIndicatorComponentView::ActivityIndicatorComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          ActivityIndicatorComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default) {}

void ActivityIndicatorComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void ActivityIndicatorComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

void ActivityIndicatorComponentView::updateProgressColor(const facebook::react::SharedColor &color) noexcept {
  if (color) {
    m_ActivityIndicatorVisual.Brush(theme()->Brush(*color));
  } else {
    m_ActivityIndicatorVisual.Brush(theme()->PlatformBrush("ProgressRingForegroundTheme"));
  }
}

void ActivityIndicatorComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto oldViewProps =
      std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(oldProps ? oldProps : viewProps());
  const auto newViewProps = std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(props);

  Super::updateProps(props, oldProps);

  // update color if needed
  if (!oldProps || newViewProps->color != oldViewProps->color) {
    updateProgressColor(newViewProps->color);
  }

  if (newViewProps->animating != oldViewProps->animating ||
      newViewProps->hidesWhenStopped != oldViewProps->hidesWhenStopped) {
    bool setHidden = (newViewProps->hidesWhenStopped && !newViewProps->animating);
    m_ActivityIndicatorVisual.IsVisible(!setHidden);

    if (!newViewProps->animating && !newViewProps->hidesWhenStopped) {
      m_ActivityIndicatorVisual.StopAnimation();
    }
  }
}

void ActivityIndicatorComponentView::FinalizeUpdates(
    winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  static constexpr float radiusSmall = 10.0f;
  static constexpr float radiusLarge = 18.0f;

  if (activityIndicatorViewProps().size == facebook::react::ActivityIndicatorViewSize::Small) {
    m_ActivityIndicatorVisual.Size(radiusSmall * m_layoutMetrics.pointScaleFactor);
  } else {
    m_ActivityIndicatorVisual.Size(radiusLarge * m_layoutMetrics.pointScaleFactor);
  }
  base_type::FinalizeUpdates(updateMask);
}

void ActivityIndicatorComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void ActivityIndicatorComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
ActivityIndicatorComponentView::createVisual() noexcept {
  auto visual = m_compContext.CreateSpriteVisual();
  m_ActivityIndicatorVisual = m_compContext.CreateActivityVisual();

  OuterVisual().InsertAt(m_ActivityIndicatorVisual, 0);
  return visual;
}

void ActivityIndicatorComponentView::onThemeChanged() noexcept {
  updateProgressColor(activityIndicatorViewProps().color);
  Super::onThemeChanged();
}

std::string ActivityIndicatorComponentView::DefaultControlType() const noexcept {
  return "progressbar";
}

facebook::react::SharedViewProps ActivityIndicatorComponentView::defaultProps() noexcept {
  static auto const defaultProps = std::make_shared<facebook::react::ActivityIndicatorViewProps const>();
  return defaultProps;
}

const facebook::react::ActivityIndicatorViewProps &ActivityIndicatorComponentView::activityIndicatorViewProps()
    const noexcept {
  return *std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(viewProps());
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
