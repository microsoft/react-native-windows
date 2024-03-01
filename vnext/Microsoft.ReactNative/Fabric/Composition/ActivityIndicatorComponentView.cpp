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
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ActivityIndicatorComponentView>(compContext, tag, reactContext);
}

ActivityIndicatorComponentView::ActivityIndicatorComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag, reactContext, ComponentViewFeatures::Default, false) {
  m_props = std::make_shared<facebook::react::ActivityIndicatorViewProps const>();
}

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
  const auto oldViewProps = std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(m_props);
  const auto newViewProps = std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(props);

  ensureVisual();

  if (newViewProps->testId != oldViewProps->testId) {
    m_visual.Comment(winrt::to_hstring(newViewProps->testId));
  }

  // update size if needed
  if (newViewProps->size != oldViewProps->size) {
    if (newViewProps->size == facebook::react::ActivityIndicatorViewSize::Small) {
      m_ActivityIndicatorVisual.Size(m_radiusSmall);
    } else {
      m_ActivityIndicatorVisual.Size(m_radiusLarge);
    }
  }

  // update color if needed
  if (!oldProps || newViewProps->color != oldViewProps->color) {
    updateProgressColor(newViewProps->color);
  }

  if (newViewProps->animating != oldViewProps->animating) {
    m_ActivityIndicatorVisual.IsVisible(newViewProps->animating);
  }

  Super::updateProps(props, oldProps);

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

void ActivityIndicatorComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void ActivityIndicatorComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  ensureVisual();

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
}

void ActivityIndicatorComponentView::prepareForRecycle() noexcept {}

facebook::react::SharedViewProps ActivityIndicatorComponentView::viewProps() noexcept {
  return m_props;
}

void ActivityIndicatorComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
    m_ActivityIndicatorVisual = m_compContext.CreateActivityVisual(); // creates COM control

    OuterVisual().InsertAt(m_ActivityIndicatorVisual, 0);
    OuterVisual().InsertAt(m_visual, 0);
  }
}

facebook::react::Tag ActivityIndicatorComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }
  return -1;
}

winrt::Microsoft::ReactNative::Composition::IVisual ActivityIndicatorComponentView::Visual() const noexcept {
  return m_visual;
}

void ActivityIndicatorComponentView::onThemeChanged() noexcept {
  updateProgressColor(std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(m_props)->color);
  Super::onThemeChanged();
}

bool ActivityIndicatorComponentView::focusable() const noexcept {
  return false;
}

std::string ActivityIndicatorComponentView::DefaultControlType() const noexcept {
  return "progressbar";
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
