
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsModalHostViewComponentView.h"

#include <Fabric/DWriteHelpers.h>
#include "../CompositionDynamicAutomationProvider.h"
#include "Composition/AutoDraw.h"
#include "Unicode.h"
#include "WinUser.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

WindowsModalHostComponentView::WindowsModalHostComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          compContext,
          tag,
          reactContext,
          (CompositionComponentViewFeatures::Default & ~CompositionComponentViewFeatures::NativeBorder)) {
  static auto const defaultProps = std::make_shared<facebook::react::ModalHostViewProps const>();
  m_props = defaultProps;
  m_visual = compContext.CreateSpriteVisual();
}

winrt::Microsoft::ReactNative::ComponentView WindowsModalHostComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<WindowsModalHostComponentView>(compContext, tag, reactContext);
}

void WindowsModalHostComponentView::mountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView & /*childComponentView*/,
    uint32_t /*index*/) noexcept {}

void WindowsModalHostComponentView::unmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView & /*childComponentView*/,
    uint32_t /*index*/) noexcept {}

void WindowsModalHostComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  Super::handleCommand(commandName, arg);
}

void WindowsModalHostComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldModalProps = *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(m_props);
  const auto &newModalProps = *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(props);

  if (oldModalProps.visible != newModalProps.visible) {
    // todo
  }

  // update BaseComponentView props
  updateTransformProps(oldModalProps, newModalProps, m_visual);
  Super::updateProps(props, oldProps);

  m_props = std::static_pointer_cast<facebook::react::ModalHostViewProps const>(props);
}

void WindowsModalHostComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  CreateDialogA();
  // TODO: RedBox placeholder for Modal (taken from unimplementedNativeViewComponent)
  // if (m_layoutMetrics.frame.size != layoutMetrics.frame.size || m_layoutMetrics.pointScaleFactor !=
  // layoutMetrics.pointScaleFactor) { // layout is never set?
  if (true) {
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
    drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    m_visual.Brush(drawingSurface);
    m_visual.Size(surfaceSize);
    m_visual.Offset({
        layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
        layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
        0.0f,
    });

    POINT offset;
    {
      ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(drawingSurface, &offset);
      if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
        d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Blue, 0.3f));
        assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);
        const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
        float oldDpiX, oldDpiY;
        d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
        d2dDeviceContext->SetDpi(dpi, dpi);

        float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor);
        offsetX = 2.0f;
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

        auto label = ::Microsoft::Common::Unicode::Utf8ToUtf16(std::string("Modal"));
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

  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

void WindowsModalHostComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void WindowsModalHostComponentView::prepareForRecycle() noexcept {}

facebook::react::SharedViewProps WindowsModalHostComponentView::viewProps() noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::Composition::IVisual WindowsModalHostComponentView::Visual() const noexcept {
  return m_visual;
}

facebook::react::Tag WindowsModalHostComponentView::hitTest(
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

bool WindowsModalHostComponentView::focusable() const noexcept {
  return false;
}

std::string WindowsModalHostComponentView::DefaultControlType() const noexcept {
  return "modal";
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
