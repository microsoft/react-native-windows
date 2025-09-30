
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "SwitchComponentView.h"
#include <AutoDraw.h>
#include <Fabric/AbiViewProps.h>
#include "CompositionDynamicAutomationProvider.h"
#include "RootComponentView.h"
#include "UiaHelpers.h"

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
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          SwitchComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background) {}

winrt::Microsoft::ReactNative::ComponentView SwitchComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<SwitchComponentView>(compContext, tag, reactContext);
}

void SwitchComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void SwitchComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

void SwitchComponentView::HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  Super::HandleCommand(args);
  if (args.Handled())
    return;
  auto commandName = args.CommandName();
  if (commandName == L"setValue") {
    // TODO - Current implementation always aligns with JS value
    // This will be needed when we move to using WinUI controls
  }
}

void SwitchComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps =
      *std::static_pointer_cast<const facebook::react::SwitchProps>(oldProps ? oldProps : viewProps());
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::SwitchProps>(props);

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor ||
      oldViewProps.thumbTintColor != newViewProps.thumbTintColor || oldViewProps.value != newViewProps.value ||
      oldViewProps.disabled != newViewProps.disabled) {
    m_visualUpdateRequired = true;
  }

  if (oldViewProps.value != newViewProps.value) {
    if (UiaClientsAreListening()) {
      winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
          EnsureUiaProvider(),
          UIA_ToggleToggleStatePropertyId,
          oldViewProps.value ? ToggleState_On : ToggleState_Off,
          newViewProps.value ? ToggleState_On : ToggleState_Off);
    }
  }

  Super::updateProps(props, oldProps);
}

void SwitchComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void SwitchComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);

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
  const auto &props = switchProps();
  auto &theme = *this->theme();
  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush defaultColor;
  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush fillColor;
  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush thumbFill;

  auto thumbWidth = SwitchConstants::thumbWidth;
  auto thumbHeight = SwitchConstants::thumbWidth;

  if (props.value) {
    if (props.disabled) {
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
    if (props.disabled) {
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

  if (props.disabled) {
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

  if (!props.disabled && props.thumbTintColor) {
    thumbFill = theme.Brush(*props.thumbTintColor);
  }

  if (!props.disabled && props.onTintColor && props.value) {
    fillColor = theme.Brush(*props.onTintColor);
  } else if (!props.disabled && props.tintColor && !props.value) {
    fillColor = theme.Brush(*props.tintColor);
  }

  // switch track - outline
  if ((!props.onTintColor && props.value) || (!props.tintColor && !props.value)) {
    m_trackVisual.StrokeThickness(std::round(SwitchConstants::trackStrokeThickness * m_layoutMetrics.pointScaleFactor));
    m_trackVisual.StrokeBrush(defaultColor);
  } else {
    m_trackVisual.StrokeThickness(0.0f);
  }

  m_trackVisual.Brush(fillColor);

  float offsetY = ((SwitchConstants::trackHeight - thumbHeight) * m_layoutMetrics.pointScaleFactor) / 2.0f;

  if (m_supressAnimationForNextFrame) {
    m_thumbVisual.AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::None);
  }

  if (props.value) {
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
    m_thumbVisual.AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::SwitchThumb);
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

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual SwitchComponentView::createVisual() noexcept {
  auto visual = m_compContext.CreateSpriteVisual();

  m_trackVisual = m_compContext.CreateRoundedRectangleVisual();
  visual.InsertAt(m_trackVisual, 0);

  m_thumbVisual = m_compContext.CreateRoundedRectangleVisual();
  m_thumbVisual.AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::SwitchThumb);
  m_trackVisual.InsertAt(m_thumbVisual, 0);

  handleScaleChange();
  return visual;
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

  if (!switchProps().disabled) {
    m_pressed = true;
    m_supressAnimationForNextFrame = true;

    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*get_strong(), winrt::Microsoft::ReactNative::FocusNavigationDirection::None);
    }

    updateVisuals();
  }
  base_type::OnPointerPressed(args);
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
  base_type::OnPointerReleased(args);
}

void SwitchComponentView::OnPointerEntered(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_hovered = true;
  m_supressAnimationForNextFrame = true;
  updateVisuals();
  base_type::OnPointerEntered(args);
}

void SwitchComponentView::OnPointerExited(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_hovered = false;
  m_supressAnimationForNextFrame = true;
  updateVisuals();
  base_type::OnPointerExited(args);
}

void SwitchComponentView::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (args.Key() == winrt::Windows::System::VirtualKey::Space ||
      args.Key() == winrt::Windows::System::VirtualKey::Enter) {
    if (toggle()) {
      args.Handled(true);
    }
  }
  Super::OnKeyUp(args);
}

bool SwitchComponentView::toggle() noexcept {
  if (switchProps().disabled || !m_eventEmitter)
    return false;

  auto switchEventEmitter = std::static_pointer_cast<facebook::react::SwitchEventEmitter const>(m_eventEmitter);

  facebook::react::SwitchEventEmitter::OnChange args;
  args.value = !(switchProps().value);
  args.target = Tag();

  switchEventEmitter->onChange(args);
  return true;
}

std::string SwitchComponentView::DefaultControlType() const noexcept {
  return "switch";
}

facebook::react::SharedViewProps SwitchComponentView::defaultProps() noexcept {
  static auto const defaultProps = std::make_shared<facebook::react::SwitchProps const>();
  return defaultProps;
}

const facebook::react::SwitchProps &SwitchComponentView::switchProps() const noexcept {
  return *std::static_pointer_cast<const facebook::react::SwitchProps>(viewProps());
}

// getToggleState method for IToggleProvider
ToggleState SwitchComponentView::getToggleState() noexcept {
  if (switchProps().value) {
    return ToggleState::ToggleState_On;
  } else {
    return ToggleState::ToggleState_Off;
  }
}

// Toggle method for IToggleProvider
void SwitchComponentView::Toggle() noexcept {
  toggle();
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
