
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "AbiCompositionViewComponentView.h"

#include <Fabric/AbiState.h>
#include <Fabric/AbiViewShadowNode.h>
#include <Fabric/DWriteHelpers.h>
#include "CompositionDynamicAutomationProvider.h"
#include "RootComponentView.h"
#include "Unicode.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

AbiCompositionViewComponentView::AbiCompositionViewComponentView(
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder)
    : Super(compContext, tag, reactContext, CompositionComponentViewFeatures::Default), m_builder(builder) {
  static auto const defaultProps = std::make_shared<::Microsoft::ReactNative::AbiViewProps const>();
  m_props = defaultProps;
  m_handle = Builder().CreateView(reactContext.Handle(), compContext);
  m_visual = Builder().CreateVisual(m_handle);
  OuterVisual().InsertAt(m_visual, 0);
}

winrt::Microsoft::ReactNative::ComponentView AbiCompositionViewComponentView::Create(
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept {
  return winrt::make<AbiCompositionViewComponentView>(reactContext, compContext, tag, builder);
}

winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder &
AbiCompositionViewComponentView::Builder() noexcept {
  return *winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder);
}

void AbiCompositionViewComponentView::mountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView & /*childComponentView*/,
    uint32_t /*index*/) noexcept {
  assert(false);
}

void AbiCompositionViewComponentView::unmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView & /*childComponentView*/,
    uint32_t /*index*/) noexcept {
  assert(false);
}

void AbiCompositionViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const ::Microsoft::ReactNative::AbiViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const ::Microsoft::ReactNative::AbiViewProps>(props);

  if (oldViewProps.testId != newViewProps.testId) {
    m_visual.Comment(winrt::to_hstring(newViewProps.testId));
  }

  updateAccessibilityProps(oldViewProps, newViewProps);
  // updateShadowProps(oldViewProps, newViewProps, m_visual);
  // updateTransformProps(oldViewProps, newViewProps, m_visual);
  Super::updateProps(props, oldProps);

  Builder().UpdateProps(m_handle, newViewProps.UserProps());

  m_props = std::static_pointer_cast<::Microsoft::ReactNative::AbiViewProps const>(props);
}

void AbiCompositionViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);

  winrt::Microsoft::ReactNative::Composition::LayoutMetrics lm;
  Builder().UpdateLayoutMetrics(
      m_handle,
      {{layoutMetrics.frame.origin.x,
        layoutMetrics.frame.origin.y,
        layoutMetrics.frame.size.width,
        layoutMetrics.frame.size.height},
       layoutMetrics.pointScaleFactor});
}

void AbiCompositionViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  m_state = winrt::make<::Microsoft::ReactNative::AbiComponentState>(state);
  Builder().UpdateState(m_handle, m_state);
}

void AbiCompositionViewComponentView::finalizeUpdates(
    winrt::Microsoft::ReactNative::implementation::RNComponentViewUpdateMask updateMask) noexcept {
  Super::finalizeUpdates(updateMask);
  Builder().FinalizeUpdates(m_handle);
}

bool AbiCompositionViewComponentView::focusable() const noexcept {
  return m_props->focusable;
}

void AbiCompositionViewComponentView::onKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  Builder().OnKeyDown(m_handle, source, args);
  Super::onKeyDown(source, args);
}

void AbiCompositionViewComponentView::onKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  Builder().OnKeyUp(m_handle, source, args);
  Super::onKeyUp(source, args);
}

void AbiCompositionViewComponentView::onCharacterReceived(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept {
  Builder().OnCharacterReceived(m_handle, source, args);
  Super::onCharacterReceived(source, args);
}

void AbiCompositionViewComponentView::onPointerEntered(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  Builder().OnPointerEntered(m_handle, args);
  Super::onPointerEntered(args);
}

void AbiCompositionViewComponentView::onPointerExited(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  Builder().OnPointerExited(m_handle, args);
  Super::onPointerExited(args);
}

void AbiCompositionViewComponentView::onPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  Builder().OnPointerPressed(m_handle, args);
  Super::onPointerPressed(args);
}

void AbiCompositionViewComponentView::onPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  Builder().OnPointerReleased(m_handle, args);
  Super::onPointerReleased(args);
}

void AbiCompositionViewComponentView::onPointerMoved(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  Builder().OnPointerMoved(m_handle, args);
  Super::onPointerMoved(args);
}

void AbiCompositionViewComponentView::onPointerWheelChanged(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  Builder().OnPointerWheelChanged(m_handle, args);
  Super::onPointerWheelChanged(args);
}

std::vector<facebook::react::ComponentDescriptorProvider>
AbiCompositionViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void AbiCompositionViewComponentView::prepareForRecycle() noexcept {}

facebook::react::SharedViewProps AbiCompositionViewComponentView::viewProps() noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::Composition::IVisual AbiCompositionViewComponentView::Visual() const noexcept {
  return m_visual;
}

facebook::react::Tag AbiCompositionViewComponentView::hitTest(
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

winrt::IInspectable AbiCompositionViewComponentView::EnsureUiaProvider() noexcept {
  if (m_uiaProvider == nullptr) {
    m_uiaProvider =
        winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>(*get_strong());
  }
  return m_uiaProvider;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
