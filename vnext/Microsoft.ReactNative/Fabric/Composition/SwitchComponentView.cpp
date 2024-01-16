
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "SwitchComponentView.h"
#include <Fabric/AbiViewProps.h>
#include "Composition/AutoDraw.h"
#include "CompositionDynamicAutomationProvider.h"
#include "RootComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

SwitchComponentView::SwitchComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(compContext, tag, reactContext, CompositionComponentViewFeatures::Default) {
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

void SwitchComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  if (commandName == "setValue") {
    // TODO - Current implementation always aligns with JS value
    // This will be needed when we move to using WinUI controls
  } else {
    Super::handleCommand(commandName, arg);
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
    m_drawingSurface = nullptr;
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
}

void SwitchComponentView::finalizeUpdates(
    winrt::Microsoft::ReactNative::implementation::RNComponentViewUpdateMask updateMask) noexcept {
  ensureDrawingSurface();
  Super::finalizeUpdates(updateMask);
}

void SwitchComponentView::Draw() noexcept {
  POINT offset;

  if (theme()->IsEmpty()) {
    return;
  }

  ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(m_drawingSurface, &offset);
  if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
    const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);
    auto &theme = *this->theme();

    if (m_props->backgroundColor) {
      d2dDeviceContext->Clear(theme.D2DColor(*m_props->backgroundColor));
    } else {
      d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
    }

    float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor);
    float offsetY = static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor);

    // https://github.com/microsoft/microsoft-ui-xaml/blob/winui2/main/dev/CommonStyles/ToggleSwitch_themeresources.xaml
    constexpr float thumbMargin = 3.0f;
    constexpr float thumbRadius = 7.0f;
    constexpr float trackWidth = 40.0f;
    constexpr float trackHeight = 20.0f;
    constexpr float trackCornerRadius = 10.0f;

    auto frame{m_layoutMetrics.frame.size};
    float trackMarginX = (frame.width - trackWidth) / 2;
    float trackMarginY = (frame.height - trackHeight) / 2;

    D2D1_RECT_F trackRect = D2D1::RectF(
        offsetX + trackMarginX,
        offsetY + trackMarginY,
        offsetX + trackMarginX + trackWidth,
        offsetY + trackMarginY + trackHeight);

    winrt::com_ptr<ID2D1SolidColorBrush> defaultBrush;

    D2D1_COLOR_F defaultColor;
    D2D1_COLOR_F fillColor;
    winrt::Microsoft::ReactNative::Composition::IBrush thumbFill;

    if (switchProps->value) {
      if (switchProps->disabled) {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOnDisabled");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOnDisabled");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOnDisabled");
      } else if (m_pressed) {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOnPressed");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOnPressed");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOnPressed");
      } else if (m_hovered) {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOnPointerOver");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOnPointerOver");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOnPointerOver");
      } else {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOn");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOn");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOn");
      }
    } else {
      if (switchProps->disabled) {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOffDisabled");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOffDisabled");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOffDisabled");
      } else if (m_pressed) {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOffPressed");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOffPressed");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOffPressed");
      } else if (m_hovered) {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOffPointerOver");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOffPointerOver");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOffPointerOver");
      } else {
        defaultColor = theme.D2DPlatformColor("ToggleSwitchStrokeOff");
        fillColor = theme.D2DPlatformColor("ToggleSwitchFillOff");
        thumbFill = theme.PlatformBrush("ToggleSwitchKnobFillOff");
      }
    }

    winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(defaultColor, defaultBrush.put()));

    if (!switchProps->disabled && switchProps->thumbTintColor) {
      thumbFill = theme.Brush(*switchProps->thumbTintColor);
    }

    if (!switchProps->disabled && switchProps->onTintColor && switchProps->value) {
      fillColor = theme.D2DColor(*switchProps->onTintColor);
    } else if (!switchProps->disabled && switchProps->tintColor && !switchProps->value) {
      fillColor = theme.D2DColor(*switchProps->tintColor);
    }

    const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
    float oldDpiX, oldDpiY;
    d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
    d2dDeviceContext->SetDpi(dpi, dpi);

    // switch track - outline
    D2D1_ROUNDED_RECT track = D2D1::RoundedRect(trackRect, trackCornerRadius, trackCornerRadius);
    if ((!switchProps->onTintColor && switchProps->value) || (!switchProps->tintColor && !switchProps->value)) {
      d2dDeviceContext->DrawRoundedRectangle(track, defaultBrush.get());
    }

    // switch track - fill
    winrt::com_ptr<ID2D1SolidColorBrush> trackBrush;
    winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(fillColor, trackBrush.put()));
    d2dDeviceContext->FillRoundedRectangle(track, trackBrush.get());

    // switch thumb - made with composition
    float thumbX = (trackMarginX + thumbMargin) * m_layoutMetrics.pointScaleFactor;
    float thumbY = (trackMarginY + thumbMargin) * m_layoutMetrics.pointScaleFactor;

    if (switchProps->value) {
      thumbX = (trackMarginX + trackWidth - thumbRadius - thumbRadius - thumbMargin) * m_layoutMetrics.pointScaleFactor;
    }

    // handles various mouse events
    if (m_pressed && !switchProps->disabled) {
      m_thumbVisual.AnimatePosition({thumbX - 0.8f, thumbY - 0.8f});
    } else if (m_hovered && !switchProps->disabled) {
      m_thumbVisual.Size(
          {thumbRadius * m_layoutMetrics.pointScaleFactor + 0.8f,
           thumbRadius * m_layoutMetrics.pointScaleFactor + 0.8f});
    } else {
      m_thumbVisual.Size(
          {thumbRadius * m_layoutMetrics.pointScaleFactor, thumbRadius * m_layoutMetrics.pointScaleFactor});
      m_thumbVisual.AnimatePosition({thumbX, thumbY});
    }

    m_thumbVisual.Brush(thumbFill);

    // Restore old dpi setting
    d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);
  }
}

void SwitchComponentView::prepareForRecycle() noexcept {}

facebook::react::SharedViewProps SwitchComponentView::viewProps() noexcept {
  return m_props;
}

void SwitchComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
    OuterVisual().InsertAt(m_visual, 0);
  }

  if (!m_thumbVisual) {
    m_thumbVisual = m_compContext.CreateSwitchThumbVisual();
    m_visual.InsertAt(m_thumbVisual.InnerVisual(), 0);
  }
}

void SwitchComponentView::ensureDrawingSurface() noexcept {
  if (!m_drawingSurface) {
    winrt::Windows::Foundation::Size surfaceSize = {
        m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
        m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor};
    m_drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
        surfaceSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    Draw();

    m_visual.Brush(m_drawingSurface);
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
  Draw();
  Super::onThemeChanged();
}

void SwitchComponentView::onPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  // Only care about primary input
  if (!args.GetCurrentPoint(-1).Properties().IsPrimary()) {
    return;
  }

  const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);

  if (!switchProps->disabled) {
    m_pressed = true;

    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*get_strong());
    }
    if (toggle()) {
      args.Handled(true);
    }

    Draw();
  }
}

void SwitchComponentView::onPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  // Only care about primary input
  if (!args.GetCurrentPoint(-1).Properties().IsPrimary()) {
    return;
  }
  m_pressed = false;
}

void SwitchComponentView::onPointerEntered(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_hovered = true;
  Draw();
}

void SwitchComponentView::onPointerExited(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_hovered = false;
  Draw();
}

void SwitchComponentView::onKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (args.Key() == winrt::Windows::System::VirtualKey::Space) {
    if (toggle()) {
      args.Handled(true);
    }
  }
  Super::onKeyUp(source, args);
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
