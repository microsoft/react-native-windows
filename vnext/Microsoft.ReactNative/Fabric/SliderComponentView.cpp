
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "SliderComponentView.h"

#include <UI.Xaml.Controls.Primitives.h>
#include <Utils/ValueUtils.h>

#include <IReactContext.h>

#include <react/renderer/components/rnwcore/EventEmitters.h>

namespace Microsoft::ReactNative {

SliderComponentView::SliderComponentView(winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : m_context(reactContext),
      m_slider(xaml::Controls::Slider()),
      m_element(winrt::make<winrt::Microsoft::ReactNative::implementation::YogaXamlPanel>(
          [&](facebook::react::Size desiredSize) {
            auto &data = m_state->getData();
            auto &originalSize = data.getSize();
            if (desiredSize.width != originalSize.width || desiredSize.height != originalSize.height) {
              m_state->updateState({data.getLayoutConstraints(), {desiredSize.width, desiredSize.height}});
            }
          },
          [&]() -> const facebook::react::LayoutConstraints & { return m_state->getData().getLayoutConstraints(); })) {
  m_element.Children().Append(m_slider);
  m_valueChangedRevoker = m_slider.ValueChanged(winrt::auto_revoke, [this](auto sender, auto args) {
    if (m_props->value != m_slider.Value()) {
      if (m_eventEmitter) {
        auto emitter = std::static_pointer_cast<const facebook::react::SliderEventEmitter>(m_eventEmitter);
        facebook::react::SliderEventEmitter::OnValueChange onValueChangeArgs;
        onValueChangeArgs.value = m_slider.Value();
        emitter->onValueChange(onValueChangeArgs);
      }
    }
  });

  static auto const defaultProps = std::make_shared<facebook::react::SliderProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
SliderComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void SliderComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
  // m_element->Children().InsertAt(index, static_cast<const BaseComponentView &>(childComponentView).Element());
}

void SliderComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
  // m_element->Children().RemoveAt(index);
}

void SliderComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldSliderProps = *std::static_pointer_cast<const facebook::react::SliderProps>(m_props);
  const auto &newSliderProps = *std::static_pointer_cast<const facebook::react::SliderProps>(props);

  if (oldSliderProps.value != newSliderProps.value) {
    m_slider.Value(newSliderProps.value);
  }

  if (oldSliderProps.maximumValue != newSliderProps.maximumValue) {
    m_slider.Maximum(newSliderProps.maximumValue);
  }

  if (oldSliderProps.minimumValue != newSliderProps.minimumValue) {
    m_slider.Minimum(newSliderProps.minimumValue);
  }

  if (oldSliderProps.disabled != newSliderProps.disabled) {
    m_slider.IsEnabled(!newSliderProps.disabled);
  }

  // TODO tint colors

  m_props = std::static_pointer_cast<facebook::react::SliderProps const>(props);
}

void SliderComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  m_state = std::static_pointer_cast<facebook::react::SliderShadowNode::ConcreteState const>(state);

  if (m_layoutConstraints != m_state->getData().getLayoutConstraints()) {
    m_layoutConstraints = m_state->getData().getLayoutConstraints();
    m_element.InvalidateMeasure();
  }
}

void SliderComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_element, layoutMetrics.frame.origin.y);

  m_element.Width(layoutMetrics.frame.size.width);
  m_element.Height(layoutMetrics.frame.size.height);
}

void SliderComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}

void SliderComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps SliderComponentView::props() noexcept {
  assert(false);
  return {};
}

const xaml::FrameworkElement SliderComponentView::Element() const noexcept {
  return m_element;
}

} // namespace Microsoft::ReactNative
