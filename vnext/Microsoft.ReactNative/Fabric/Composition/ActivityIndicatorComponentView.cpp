// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ActivityIndicatorComponentView.h"
#include "CompositionDynamicAutomationProvider.h"

#include <Windows.UI.Composition.h>
#include <Windows.h>
#include "CompositionContextHelper.h"

namespace Microsoft::ReactNative {

std::shared_ptr<ActivityIndicatorComponentView> ActivityIndicatorComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return std::shared_ptr<ActivityIndicatorComponentView>(
      new ActivityIndicatorComponentView(compContext, tag, reactContext));
}

ActivityIndicatorComponentView::ActivityIndicatorComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag), m_context(reactContext) {
  m_props = std::make_shared<facebook::react::ActivityIndicatorViewProps const>();
}

void ActivityIndicatorComponentView::mountChildComponentView(
    IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void ActivityIndicatorComponentView::unmountChildComponentView(
    IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void ActivityIndicatorComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  Super::handleCommand(commandName, arg);
}

void ActivityIndicatorComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto oldViewProps = std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(m_props);
  const auto newViewProps = std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(props);

  ensureVisual();

  // update color if needed
  if (newViewProps->color && (!oldProps || newViewProps->color != oldViewProps->color)) {
    m_ActivityIndicatorVisual.Color(newViewProps->color.AsWindowsColor());
  }

  if (newViewProps->animating != oldViewProps->animating) {
    m_ActivityIndicatorVisual.IsVisible(newViewProps->animating);
  }

  updateBorderProps(*oldViewProps, *newViewProps);
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

  updateBorderLayoutMetrics(layoutMetrics, *m_props);
  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
}

void ActivityIndicatorComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}

void ActivityIndicatorComponentView::prepareForRecycle() noexcept {}

facebook::react::Props::Shared ActivityIndicatorComponentView::props() noexcept {
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
    return tag();
  }
  return -1;
}

winrt::Microsoft::ReactNative::Composition::IVisual ActivityIndicatorComponentView::Visual() const noexcept {
  return m_visual;
}

bool ActivityIndicatorComponentView::focusable() const noexcept {
  return false;
}

std::string ActivityIndicatorComponentView::DefaultControlType() const noexcept {
  return "progressbar";
}

} // namespace Microsoft::ReactNative
