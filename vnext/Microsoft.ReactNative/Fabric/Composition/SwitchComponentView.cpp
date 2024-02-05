
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "SwitchComponentView.h"
#include <Fabric/AbiViewProps.h>
#include "Composition/AutoDraw.h"
#include "CompositionDynamicAutomationProvider.h"
#include "RootComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

namespace SwitchConstants {
// https://github.com/microsoft/microsoft-ui-xaml/blob/winui3/release/1.5-experimental1/controls/dev/CommonStyles/ToggleSwitch_themeresources.xaml
constexpr float thumbMargin = 4.0f;
constexpr float thumbWidth = 12.0f;
constexpr float thumbWidthHover = 14.0f;
constexpr float thumbWidthPressed = 17.0f;
constexpr float trackWidth = 40.0f;
constexpr float trackHeight = 20.0f;
constexpr float trackCornerRadius = 10.0f;
constexpr float trackStrokeThickness = 1.0f;
} // namespace SwitchConstants

SwitchComponentView::SwitchComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(compContext, tag, reactContext, CompositionComponentViewFeatures::Default, false) {
  m_props = std::make_shared<facebook::react::SwitchProps const>();
}

winrt::Microsoft::ReactNative::ComponentView SwitchComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<SwitchComponentView>(compContext, tag, reactContext);
}

void SwitchComponentView::mountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void SwitchComponentView::unmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void SwitchComponentView::HandleCommand(
    winrt::hstring commandName,
    const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept {
  if (commandName == L"setValue") {
    // TODO - Current implementation always aligns with JS value
    // This will be needed when we move to using WinUI controls
  } else {
    Super::HandleCommand(commandName, args);
  }
}

void SwitchComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::SwitchProps>(props);

  ensureVisual();

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor ||
      oldViewProps.thumbTintColor != newViewProps.thumbTintColor || oldViewProps.value != newViewProps.value ||
      oldViewProps.disabled != newViewProps.disabled) {
    m_visualUpdateRequired = true;
  }
  if (oldViewProps.testId != newViewProps.testId) {
    m_visual.Comment(winrt::to_hstring(newViewProps.testId));
  }

  // update BaseComponentView props
  updateTransformProps(oldViewProps, newViewProps, m_visual);
  Super::updateProps(props, oldProps);
  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

void SwitchComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void SwitchComponentView::updateLayoutMetrics(
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

  if (oldLayoutMetrics.pointScaleFactor != layoutMetrics.pointScaleFactor) {
    handleScaleChange();
  }

  auto frame{m_layoutMetrics.frame.size};
  m_trackVisual.Offset(
      {(frame.width - SwitchConstants::trackWidth) * m_layoutMetrics.pointScaleFactor / 2.0f,
       (frame.height - SwitchConstants::trackHeight) * m_layoutMetrics.pointScaleFactor / 2.0f,
       0.0f});
}

void SwitchComponentView::handleScaleChange() noexcept {
  m_trackVisual.Size(
      {SwitchConstants::trackWidth * m_layoutMetrics.pointScaleFactor,
       SwitchConstants::trackHeight * m_layoutMetrics.pointScaleFactor});

  m_trackVisual.CornerRadius(
      {SwitchConstants::trackCornerRadius * m_layoutMetrics.pointScaleFactor,
       SwitchConstants::trackCornerRadius * m_layoutMetrics.pointScaleFactor});

  m_visualUpdateRequired = true;
}

void SwitchComponentView::updateVisuals() noexcept {
  const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);
  auto &theme = *this->theme();
  winrt::Microsoft::ReactNative::Composition::IBrush defaultColor;
  winrt::Microsoft::ReactNative::Composition::IBrush fillColor;
  winrt::Microsoft::ReactNative::Composition::IBrush thumbFill;

  auto thumbWidth = SwitchConstants::thumbWidth;
  auto thumbHeight = SwitchConstants::thumbWidth;

  if (switchProps->value) {
    if (switchProps->disabled) {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOnDisabled");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOnDisabled");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOnDisabled");
    } else if (m_pressed) {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOnPressed");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOnPressed");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOnPressed");
    } else if (m_hovered) {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOnPointerOver");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOnPointerOver");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOnPointerOver");
    } else {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOn");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOn");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOn");
    }
  } else {
    if (switchProps->disabled) {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOffDisabled");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOffDisabled");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOffDisabled");
    } else if (m_pressed) {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOffPressed");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOffPressed");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOffPressed");
    } else if (m_hovered) {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOffPointerOver");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOffPointerOver");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOffPointerOver");
    } else {
      defaultColor = theme.PlatformBrush("ToggleSwitchStrokeOff");
      fillColor = theme.PlatformBrush("ToggleSwitchFillOff");
      thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOff");
    }
  }

  if (switchProps->disabled) {
    thumbWidth = SwitchConstants::thumbWidth;
  } else if (m_pressed) {
    thumbWidth = SwitchConstants::thumbWidthPressed;
    thumbHeight = SwitchConstants::thumbWidthHover;
  } else if (m_hovered) {
    thumbWidth = SwitchConstants::thumbWidthHover;
    thumbHeight = SwitchConstants::thumbWidthHover;
  } else {
    thumbWidth = SwitchConstants::thumbWidth;
  }

  if (!switchProps->disabled && switchProps->thumbTintColor) {
    thumbFill = theme.Brush(*switchProps->thumbTintColor);
  }

  if (!switchProps->disabled && switchProps->onTintColor && switchProps->value) {
    fillColor = theme.Brush(*switchProps->onTintColor);
  } else if (!switchProps->disabled && switchProps->tintColor && !switchProps->value) {
    fillColor = theme.Brush(*switchProps->tintColor);
  }

  // switch track - outline
  if ((!switchProps->onTintColor && switchProps->value) || (!switchProps->tintColor && !switchProps->value)) {
    m_trackVisual.StrokeThickness(std::round(SwitchConstants::trackStrokeThickness * m_layoutMetrics.pointScaleFactor));
    m_trackVisual.StrokeBrush(defaultColor);
  } else {
    m_trackVisual.StrokeThickness(0.0f);
  }

  m_trackVisual.Brush(fillColor);

  float offsetY = ((SwitchConstants::trackHeight - thumbHeight) * m_layoutMetrics.pointScaleFactor) / 2.0f;

  if (m_supressAnimationForNextFrame) {
    m_thumbVisual.AnimationClass(winrt::Microsoft::ReactNative::Composition::AnimationClass::None);
  }

  if (switchProps->value) {
    m_thumbVisual.Offset(
        {(SwitchConstants::trackWidth - (SwitchConstants::thumbMargin + thumbWidth)) * m_layoutMetrics.pointScaleFactor,
         offsetY,
         0.0f});
  } else {
    m_thumbVisual.Offset({SwitchConstants::thumbMargin * m_layoutMetrics.pointScaleFactor, offsetY, 0.0f});
  }
  m_thumbVisual.Size({thumbWidth * m_layoutMetrics.pointScaleFactor, thumbHeight * m_layoutMetrics.pointScaleFactor});
  m_thumbVisual.CornerRadius(
      {(thumbHeight * m_layoutMetrics.pointScaleFactor) / 2.0f,
       (thumbHeight * m_layoutMetrics.pointScaleFactor) / 2.0f});
  m_thumbVisual.Brush(thumbFill);

  if (m_supressAnimationForNextFrame) {
    m_thumbVisual.AnimationClass(winrt::Microsoft::ReactNative::Composition::AnimationClass::SwitchThumb);
    m_supressAnimationForNextFrame = false;
  }
}

void SwitchComponentView::FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  if (m_visualUpdateRequired) {
    m_visualUpdateRequired = false;
    updateVisuals();
  }

  base_type::FinalizeUpdates(updateMask);
}

void SwitchComponentView::prepareForRecycle() noexcept {}

facebook::react::SharedViewProps SwitchComponentView::viewProps() noexcept {
  return m_props;
}

void SwitchComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
    OuterVisual().InsertAt(m_visual, 0);

    m_trackVisual = m_compContext.CreateRoundedRectangleVisual();
    m_visual.InsertAt(m_trackVisual, 0);

    m_thumbVisual = m_compContext.CreateRoundedRectangleVisual();
    m_thumbVisual.AnimationClass(winrt::Microsoft::ReactNative::Composition::AnimationClass::SwitchThumb);
    m_trackVisual.InsertAt(m_thumbVisual, 0);

    handleScaleChange();
  }
}

facebook::react::Tag SwitchComponentView::hitTest(
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

winrt::Microsoft::ReactNative::Composition::IVisual SwitchComponentView::Visual() const noexcept {
  return m_visual;
}

void SwitchComponentView::onThemeChanged() noexcept {
  updateVisuals();
  Super::onThemeChanged();
}

void SwitchComponentView::OnPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  // Only care about primary input
  if (!args.GetCurrentPoint(-1).Properties().IsPrimary()) {
    return;
  }

  const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);

  if (!switchProps->disabled) {
    m_pressed = true;
    m_supressAnimationForNextFrame = true;

    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*get_strong());
    }

    updateVisuals();
  }
}

void SwitchComponentView::OnPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  // Only care about primary input
  if (!args.GetCurrentPoint(-1).Properties().IsPrimary()) {
    return;
  }

  if (toggle()) {
    args.Handled(true);
  } else {
    m_supressAnimationForNextFrame = true;
  }

  m_pressed = false;
}

void SwitchComponentView::OnPointerEntered(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_hovered = true;
  m_supressAnimationForNextFrame = true;
  updateVisuals();
}

void SwitchComponentView::OnPointerExited(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_hovered = false;
  m_supressAnimationForNextFrame = true;
  updateVisuals();
}

void SwitchComponentView::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (args.Key() == winrt::Windows::System::VirtualKey::Space) {
    if (toggle()) {
      args.Handled(true);
    }
  }
  Super::OnKeyUp(source, args);
}

bool SwitchComponentView::toggle() noexcept {
  const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);

  if (switchProps->disabled || !m_eventEmitter)
    return false;

  auto switchEventEmitter = std::static_pointer_cast<facebook::react::SwitchEventEmitter const>(m_eventEmitter);

  facebook::react::SwitchEventEmitter::OnChange args;
  args.value = !(switchProps->value);
  args.target = Tag();

  switchEventEmitter->onChange(args);
  return true;
}

bool SwitchComponentView::focusable() const noexcept {
  return m_props->focusable;
}

std::string SwitchComponentView::DefaultControlType() const noexcept {
  return "switch";
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
