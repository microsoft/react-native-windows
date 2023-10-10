
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "SwitchComponentView.h"
#include "Composition/AutoDraw.h"
#include "CompositionDynamicAutomationProvider.h"
#include "RootComponentView.h"

namespace Microsoft::ReactNative {

SwitchComponentView::SwitchComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag), m_context(reactContext) {
  m_props = std::make_shared<facebook::react::SwitchProps const>();
}

std::shared_ptr<SwitchComponentView> SwitchComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return std::shared_ptr<SwitchComponentView>(new SwitchComponentView(compContext, tag, reactContext));
}

void SwitchComponentView::mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
}

void SwitchComponentView::unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
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

  // update BaseComponentView props
  updateShadowProps(oldViewProps, newViewProps, m_visual);
  updateTransformProps(oldViewProps, newViewProps, m_visual);
  updateBorderProps(oldViewProps, newViewProps);
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

  updateBorderLayoutMetrics(layoutMetrics, *m_props);
  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
}

void SwitchComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  ensureDrawingSurface();

  if (m_needsBorderUpdate) {
    m_needsBorderUpdate = false;
    UpdateSpecialBorderLayers(m_layoutMetrics, *m_props);
  }
}

void SwitchComponentView::Draw() noexcept {
  POINT offset;

  ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(m_drawingSurface, &offset);
  if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
    const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
    if (m_props->backgroundColor) {
      d2dDeviceContext->Clear(m_props->backgroundColor.AsD2DColor());
    }

    float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor);
    float offsetY = static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor);

    // https://github.com/microsoft/microsoft-ui-xaml/blob/main/dev/CommonStyles/ToggleSwitch_themeresources.xaml
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

    D2D1_COLOR_F defaultColor =
        switchProps->disabled ? facebook::react::greyColor().AsD2DColor() : facebook::react::blackColor().AsD2DColor();

    winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(defaultColor, defaultBrush.put()));

    winrt::Windows::UI::Color thumbColor;
    if (!switchProps->disabled && switchProps->thumbTintColor) {
      thumbColor = switchProps->thumbTintColor.AsWindowsColor();
    } else {
      thumbColor = winrt::Windows::UI::Colors::Gray();
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
    if (!switchProps->disabled && switchProps->onTintColor && switchProps->value) {
      winrt::check_hresult(
          d2dDeviceContext->CreateSolidColorBrush(switchProps->onTintColor.AsD2DColor(), trackBrush.put()));
      d2dDeviceContext->FillRoundedRectangle(track, trackBrush.get());
    } else if (!switchProps->disabled && switchProps->tintColor && !switchProps->value) {
      winrt::check_hresult(
          d2dDeviceContext->CreateSolidColorBrush(switchProps->tintColor.AsD2DColor(), trackBrush.put()));
      d2dDeviceContext->FillRoundedRectangle(track, trackBrush.get());
    }

    // switch thumb - made with composition
    float thumbX = (trackMarginX + thumbMargin) * m_layoutMetrics.pointScaleFactor;
    float thumbY = (trackMarginY + thumbMargin) * m_layoutMetrics.pointScaleFactor;

    if (switchProps->value) {
      thumbX = (trackMarginX + trackWidth - thumbRadius - thumbRadius - thumbMargin) * m_layoutMetrics.pointScaleFactor;
    }

    m_thumbVisual.Size(
        {thumbRadius * m_layoutMetrics.pointScaleFactor, thumbRadius * m_layoutMetrics.pointScaleFactor});
    m_thumbVisual.Position({thumbX, thumbY});
    m_thumbVisual.Color(thumbColor);

    // Restore old dpi setting
    d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);
  }
}

void SwitchComponentView::prepareForRecycle() noexcept {}

facebook::react::Props::Shared SwitchComponentView::props() noexcept {
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
    return tag();
  }

  return -1;
}

winrt::Microsoft::ReactNative::Composition::IVisual SwitchComponentView::Visual() const noexcept {
  return m_visual;
}

void SwitchComponentView::onPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);

  if (!switchProps->disabled) {
    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*this);
    }
    if (toggle()) {
      args.Handled(true);
    }
  }
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
  args.target = tag();

  switchEventEmitter->onChange(args);
  return true;
}

bool SwitchComponentView::focusable() const noexcept {
  return m_props->focusable;
}

std::string SwitchComponentView::DefaultControlType() const noexcept {
  return "switch";
}

} // namespace Microsoft::ReactNative
