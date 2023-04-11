// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ActivityIndicatorComponentView.h"
#include "CompositionDynamicAutomationProvider.h"

namespace Microsoft::ReactNative {

ActivityIndicatorComponentView::ActivityIndicatorComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag), m_context(reactContext) {
  m_props = std::make_shared<facebook::react::ActivityIndicatorViewProps const>();
}

std::shared_ptr<ActivityIndicatorComponentView> ActivityIndicatorComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return std::shared_ptr<ActivityIndicatorComponentView>(
      new ActivityIndicatorComponentView(compContext, tag, reactContext));
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
  if (commandName == "setValue") {
    // TODO - Current implementation always aligns with JS value
    // This will be needed when we move to using WinUI controls
  } else {
    Super::handleCommand(commandName, arg);
  }
}

void ActivityIndicatorComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(props);

  ensureVisual();

  updateBorderProps(oldViewProps, newViewProps);
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

void ActivityIndicatorComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  ensureDrawingSurface();
}

void ActivityIndicatorComponentView::Draw() noexcept {
  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
  POINT offset;

  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> drawingSurfaceInterop;
  m_drawingSurface.as(drawingSurfaceInterop);

  if (CheckForDeviceRemoved(drawingSurfaceInterop->BeginDraw(d2dDeviceContext.put(), &offset))) {
    const auto activityIndicatorProps =
        std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(m_props);

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

    float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor);
    float offsetY = static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor);

    const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
    float oldDpiX, oldDpiY;
    d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
    d2dDeviceContext->SetDpi(dpi, dpi);

    // constants
    constexpr float radiusSmall = 8.0f;
    constexpr float radiusLarge = 16.0f;
    constexpr float ringWidth = 2.0f;

    // create defaultBrush (TODO: figure out light/dark theme)
    winrt::com_ptr<ID2D1SolidColorBrush> defaultBrush;
    winrt::check_hresult(
        d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), defaultBrush.put()));

    // get loading colors
    winrt::com_ptr<ID2D1SolidColorBrush> circleBrush;
    if (activityIndicatorProps->color) {
      winrt::check_hresult(
          d2dDeviceContext->CreateSolidColorBrush(activityIndicatorProps->color.AsD2DColor(), circleBrush.put()));
    } else {
      circleBrush = defaultBrush;
    }

    // draw ring - this isn't visable in Paper version but may be helpful when animating
    D2D1_ELLIPSE ring;
    if (activityIndicatorProps->size ==
        facebook::react::ActivityIndicatorViewSize::Small) { // (TODO: figure out why the size is always small? )
      D2D1_POINT_2F center = D2D1 ::Point2F(offsetX + radiusSmall + ringWidth, offsetY + radiusSmall + ringWidth);
      ring = D2D1::Ellipse(center, radiusSmall, radiusSmall);
    } else {
      D2D1_POINT_2F center = D2D1 ::Point2F(offsetX + radiusLarge, offsetY + radiusLarge);
      ring = D2D1::Ellipse(center, radiusLarge, radiusLarge);
    }
    d2dDeviceContext->DrawEllipse(ring, defaultBrush.get(), ringWidth);

    // draw loading circles
    D2D1_ELLIPSE circle1, circle2, circle3, circle4, circle5;
    circle1 =
        D2D1::Ellipse(D2D1::Point2F((offsetX + radiusSmall + ringWidth), (offsetY + ringWidth)), ringWidth, ringWidth);
    circle2 = D2D1::Ellipse(
        D2D1::Point2F(
            (offsetX + radiusSmall + ringWidth) + (radiusSmall)*cos(305.0f * 3.14159f / 180.0f),
            (offsetY + radiusSmall + ringWidth) + (radiusSmall)*sin(305.0f * 3.14159f / 180.0f)),
        ringWidth,
        ringWidth);
    circle3 = D2D1::Ellipse(
        D2D1::Point2F(offsetX + radiusSmall + ringWidth + radiusSmall, offsetY + radiusSmall + ringWidth),
        ringWidth,
        ringWidth);
    circle4 = D2D1::Ellipse(
        D2D1::Point2F(
            (offsetX + radiusSmall + ringWidth) + (radiusSmall)*cos(105.0f * 3.14159f / 180.0f),
            (offsetY + radiusSmall + ringWidth) + (radiusSmall)*sin(105.0f * 3.14159f / 180.0f)),
        ringWidth,
        ringWidth);
    circle5 = D2D1::Ellipse(
        D2D1::Point2F(
            (offsetX + radiusSmall + ringWidth) + (radiusSmall)*cos(150.0f * 3.14159f / 180.0f),
            (offsetY + radiusSmall + ringWidth) + (radiusSmall)*sin(150.0f * 3.14159f / 180.0f)),
        ringWidth,
        ringWidth);
    d2dDeviceContext->FillEllipse(circle1, circleBrush.get());
    d2dDeviceContext->FillEllipse(circle2, circleBrush.get());
    d2dDeviceContext->FillEllipse(circle3, circleBrush.get());
    d2dDeviceContext->FillEllipse(circle4, circleBrush.get());
    d2dDeviceContext->FillEllipse(circle5, circleBrush.get());

    // Restore old dpi setting
    d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    winrt::check_hresult(drawingSurfaceInterop->EndDraw());
  }
}

void ActivityIndicatorComponentView::prepareForRecycle() noexcept {}

facebook::react::Props::Shared ActivityIndicatorComponentView::props() noexcept {
  return m_props;
}

void ActivityIndicatorComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
    OuterVisual().InsertAt(m_visual, 0);
  }
}

void ActivityIndicatorComponentView::ensureDrawingSurface() noexcept {
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

facebook::react::Tag ActivityIndicatorComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
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

winrt::Microsoft::ReactNative::Composition::IVisual ActivityIndicatorComponentView::Visual() const noexcept {
  return m_visual;
}

bool ActivityIndicatorComponentView::focusable() const noexcept {
  return false;
}

} // namespace Microsoft::ReactNative
