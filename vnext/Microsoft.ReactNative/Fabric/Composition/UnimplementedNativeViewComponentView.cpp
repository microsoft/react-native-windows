
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "UnimplementedNativeViewComponentView.h"

#include <AutoDraw.h>
#include <Fabric/DWriteHelpers.h>
#include "Unicode.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

UnimplementedNativeViewComponentView::UnimplementedNativeViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          {}, // default ViewProps
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default &
              ~(ComponentViewFeatures::Background | ComponentViewFeatures::ShadowProps |
                ComponentViewFeatures::NativeBorder)) {
  m_labelVisual = compContext.CreateSpriteVisual();
  OuterVisual().InsertAt(m_labelVisual, 0);
}

winrt::Microsoft::ReactNative::ComponentView UnimplementedNativeViewComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<UnimplementedNativeViewComponentView>(compContext, tag, reactContext);
}

void UnimplementedNativeViewComponentView::HandleCommand(
    const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  // Do not call base to avoid unknown command asserts
}

void UnimplementedNativeViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
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
    auto drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
        surfaceSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    drawingSurface.HorizontalAlignmentRatio(0.f);
    drawingSurface.VerticalAlignmentRatio(0.f);
    drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
    m_labelVisual.Brush(drawingSurface);
    m_labelVisual.Size(surfaceSize);
    m_labelVisual.Offset({
        layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
        layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
        0.0f,
    });

    POINT offset;
    {
      ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(
          drawingSurface, m_layoutMetrics.pointScaleFactor, &offset);
      if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
        d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Red, 0.3f));
        assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);

        float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor);
        float offsetY = static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor);

        winrt::com_ptr<IDWriteTextFormat> spTextFormat;
        winrt::check_hresult(::Microsoft::ReactNative::DWriteFactory()->CreateTextFormat(
            L"Segoe UI",
            nullptr, // Font collection (nullptr sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            12,
            L"",
            spTextFormat.put()));

        winrt::com_ptr<ID2D1SolidColorBrush> textBrush;
        winrt::check_hresult(
            d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.put()));

        const D2D1_RECT_F rect = {
            static_cast<float>(offset.x), static_cast<float>(offset.y), width + offset.x, height + offset.y};
        // const D2D1_RECT_F rect = {0.f, 0.f, width, height};

        auto props = std::static_pointer_cast<facebook::react::UnimplementedNativeViewProps const>(viewProps());
        auto label = ::Microsoft::Common::Unicode::Utf8ToUtf16(std::string("Unimplemented component: ") + props->name);
        d2dDeviceContext->DrawText(
            label.c_str(),
            static_cast<UINT32>(label.length()),
            spTextFormat.get(),
            rect,
            textBrush.get(),
            D2D1_DRAW_TEXT_OPTIONS_NONE,
            DWRITE_MEASURING_MODE_NATURAL);
      }
    }
  }

  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
