
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ActivityIndicatorComponentView.h"

#include <Fabric/DWriteHelpers.h>
#include "CompositionDynamicAutomationProvider.h"
#include "Unicode.h"

namespace Microsoft::ReactNative {

ActivityIndicatorComponentView::ActivityIndicatorComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {
  static auto const defaultProps = std::make_shared<facebook::react::ActivityIndicatorViewProps const>();
  m_props = defaultProps;
  m_visual = compContext.CreateSpriteVisual();
}

std::shared_ptr<ActivityIndicatorComponentView> ActivityIndicatorComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag) noexcept {
  return std::shared_ptr<ActivityIndicatorComponentView>(
      new ActivityIndicatorComponentView(compContext, tag));
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

void ActivityIndicatorComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  m_props = std::static_pointer_cast<facebook::react::ActivityIndicatorViewProps const>(props);
}

void ActivityIndicatorComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {

  // Set Position & Size Properties
  // auto m_visual = m_compContext.CreateSpriteVisual(); // not sure if we need this
  // OuterVisual().InsertAt(m_visual, 0); // not sure if we need this
  
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  if (m_layoutMetrics.frame.size != layoutMetrics.frame.size ||
      m_layoutMetrics.pointScaleFactor != layoutMetrics.pointScaleFactor) {
    // Always make visual a min size, so that even if its laid out at zero size, its clear an unimplemented view was
    // rendered
    float width = std::max(m_layoutMetrics.frame.size.width, 200.0f);
    float height = std::max(m_layoutMetrics.frame.size.width, 50.0f);

    winrt::Windows::Foundation::Size surfaceSize = {
        width * m_layoutMetrics.pointScaleFactor, height * m_layoutMetrics.pointScaleFactor};
    auto drawingSurface = m_compContext.CreateDrawingSurface(
        surfaceSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    auto surfaceBrush = m_compContext.CreateSurfaceBrush(drawingSurface);
    // surfaceBrush.HorizontalAlignmentRatio(0.f);
    // surfaceBrush.VerticalAlignmentRatio(0.f);
    // surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    m_visual.Brush(surfaceBrush);
    m_visual.Size(surfaceSize);
    m_visual.Offset({
        layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
        layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
        0.0f,
    });

    winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
    POINT offset;

    winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> drawingSurfaceInterop;
    drawingSurface.as(drawingSurfaceInterop);

    if (CheckForDeviceRemoved(drawingSurfaceInterop->BeginDraw(d2dDeviceContext.put(), &offset))) {
      d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Red, 0.3f));
      assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);
      const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
      float oldDpiX, oldDpiY;
      d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
      d2dDeviceContext->SetDpi(dpi, dpi);

      float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor); // offset is same as switch
      float offsetY = static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor); // offset is same as switch

      // MY CODE SLAY ---------------------------------------------------------------------
      constexpr float radius = 16.0f;
      constexpr float ringWidth = 1.0f;

      winrt::com_ptr<ID2D1SolidColorBrush> greyBrush;
      winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DimGray), greyBrush.put()));

      D2D1_POINT_2F center = D2D1 ::Point2F(offsetX + radius, offsetY + radius);
      D2D1_ELLIPSE outerCircle = D2D1::Ellipse(center, radius, radius);
      d2dDeviceContext->FillEllipse(outerCircle, greyBrush.get());

      // UNSLAY -------------------------------------------------------------------------------

      winrt::check_hresult(drawingSurfaceInterop->EndDraw());
    }
  }
  m_layoutMetrics = layoutMetrics;
}

void ActivityIndicatorComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void ActivityIndicatorComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}

void ActivityIndicatorComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ActivityIndicatorComponentView::props() noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::Composition::IVisual ActivityIndicatorComponentView::Visual() const noexcept {
  return m_visual;
}

winrt::Microsoft::ReactNative::Composition::IVisual ActivityIndicatorComponentView::OuterVisual() const noexcept {
  return m_visual;
}

facebook::react::Tag ActivityIndicatorComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt) const noexcept {
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

} // namespace Microsoft::ReactNative

