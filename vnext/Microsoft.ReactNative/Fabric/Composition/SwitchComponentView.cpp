
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "SwitchComponentView.h"
#include "CompositionDynamicAutomationProvider.h"

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
}

void SwitchComponentView::Draw() noexcept {
  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
  POINT offset;

  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> drawingSurfaceInterop;
  m_drawingSurface.as(drawingSurfaceInterop);

  if (CheckForDeviceRemoved(drawingSurfaceInterop->BeginDraw(d2dDeviceContext.put(), &offset))) {
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

    // switchProps->value = false
    float thumbX = trackRect.left + thumbMargin + thumbRadius;

    if (switchProps->value) {
      thumbX = trackRect.right - thumbMargin - thumbRadius;
    }

    winrt::com_ptr<ID2D1SolidColorBrush> defaultBrush;

    D2D1_COLOR_F defaultColor =
        switchProps->disabled ? facebook::react::greyColor().AsD2DColor() : facebook::react::blackColor().AsD2DColor();

    winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(defaultColor, defaultBrush.put()));

    winrt::com_ptr<ID2D1SolidColorBrush> thumbBrush;
    if (!switchProps->disabled && switchProps->thumbTintColor) {
      winrt::check_hresult(
          d2dDeviceContext->CreateSolidColorBrush(switchProps->thumbTintColor.AsD2DColor(), thumbBrush.put()));
    } else {
      thumbBrush = defaultBrush;
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

    // switch thumb
    D2D1_POINT_2F thumbCenter = D2D1 ::Point2F(thumbX, (trackRect.top + trackRect.bottom) / 2);
    D2D1_ELLIPSE thumb = D2D1::Ellipse(thumbCenter, thumbRadius, thumbRadius);
    d2dDeviceContext->FillEllipse(thumb, thumbBrush.get());

    // Restore old dpi setting
    d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    winrt::check_hresult(drawingSurfaceInterop->EndDraw());
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
}

void SwitchComponentView::ensureDrawingSurface() noexcept {
  if (!m_drawingSurface) {
    winrt::Windows::Foundation::Size surfaceSize = {
        m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
        m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor};
    m_drawingSurface = m_compContext.CreateDrawingSurface(
        surfaceSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    Draw();

    auto surfaceBrush = m_compContext.CreateSurfaceBrush(m_drawingSurface);

    m_visual.Brush(surfaceBrush);
  }
}

facebook::react::Tag SwitchComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
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

int64_t SwitchComponentView::sendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_POINTERDOWN: {
      const auto switchProps = std::static_pointer_cast<const facebook::react::SwitchProps>(m_props);

      if (!switchProps->disabled && m_eventEmitter) {
        auto switchEventEmitter = std::static_pointer_cast<facebook::react::SwitchEventEmitter const>(m_eventEmitter);

        facebook::react::SwitchEventEmitter::OnChange args;
        args.value = !(switchProps->value);
        args.target = tag();

        switchEventEmitter->onChange(args);
      }
      break;
    }
  }

  return 0;
}

bool SwitchComponentView::focusable() const noexcept {
  return m_props->focusable;
}

} // namespace Microsoft::ReactNative