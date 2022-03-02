
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompParagraphComponentView.h"

#include <Utils/ValueUtils.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <unicode.h>
#include "CompHelpers.h"

namespace Microsoft::ReactNative {

CompParagraphComponentView::CompParagraphComponentView() {
  static auto const defaultProps = std::make_shared<facebook::react::ParagraphProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
CompParagraphComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void CompParagraphComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  auto v = static_cast<const CompParagraphComponentView &>(childComponentView);
  assert(false);
  // m_element.Children().InsertAt(index, v.Element());
}

void CompParagraphComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  // m_element.Children().RemoveAt(index);
}

void CompParagraphComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(props);

  if (oldViewProps.textAttributes.foregroundColor != newViewProps.textAttributes.foregroundColor) {
    m_requireRedraw = true;
  }

  if (oldViewProps.textAttributes.fontSize != newViewProps.textAttributes.fontSize) {
    m_textFormat = nullptr;
  }

  /*
  if (oldViewProps.textAttributes.fontWeight != newViewProps.textAttributes.fontWeight) {
    m_element.FontWeight(
        winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(newViewProps.textAttributes.fontWeight.value_or(
            static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR)))});
  }

  if (oldViewProps.textAttributes.fontStyle != newViewProps.textAttributes.fontStyle) {
    switch (newViewProps.textAttributes.fontStyle.value_or(facebook::react::FontStyle::Normal)) {
      case facebook::react::FontStyle::Italic:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Italic);
        break;
      case facebook::react::FontStyle::Normal:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Normal);
        break;
      case facebook::react::FontStyle::Oblique:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Oblique);
        break;
      default:
        assert(false);
    }
  }

  if (oldViewProps.textAttributes.fontFamily != newViewProps.textAttributes.fontFamily) {
    if (newViewProps.textAttributes.fontFamily.empty())
      m_element.FontFamily(xaml::Media::FontFamily(L"Segoe UI"));
    else
      m_element.FontFamily(
          xaml::Media::FontFamily(Microsoft::Common::Unicode::Utf8ToUtf16(newViewProps.textAttributes.fontFamily)));
  }
  */

  if (oldViewProps.textAttributes.alignment != newViewProps.textAttributes.alignment) {
    updateTextAlignment(newViewProps.textAttributes.alignment);
  }

  m_props = std::static_pointer_cast<facebook::react::ParagraphProps const>(props);
}

void CompParagraphComponentView::updateEventEmitter(
    facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}

void CompParagraphComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ParagraphShadowNode::ConcreteState const>(state);

  // Only handle single/empty fragments right now -- ignore the other fragments
  if (newState.getData().attributedString.getFragments().size()) {
    auto firstFragment = newState.getData().attributedString.getFragments()[0];
    m_text = Microsoft::Common::Unicode::Utf8ToUtf16(firstFragment.string);
    m_fontSize = firstFragment.textAttributes.fontSize * firstFragment.textAttributes.fontSizeMultiplier;
    m_fontFamily = firstFragment.textAttributes.fontFamily.empty()
        ? L"Segoe UI"
        : Microsoft::Common::Unicode::Utf8ToUtf16(firstFragment.textAttributes.fontFamily);
    m_fontWeight = static_cast<DWRITE_FONT_WEIGHT>(firstFragment.textAttributes.fontWeight.value_or(
        static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR)));
    m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
    if (firstFragment.textAttributes.fontStyle == facebook::react::FontStyle::Italic) {
      m_fontStyle = DWRITE_FONT_STYLE_ITALIC;
    } else if (firstFragment.textAttributes.fontStyle == facebook::react::FontStyle::Oblique) {
      m_fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
    }
  } else {
    m_fontFamily = L"Segoe UI";
    m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
    m_text = L"";
    m_fontSize = 12.f;
    m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
  }

  m_textLayout = nullptr;
}
void CompParagraphComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  ensureVisual();

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  m_layoutMetrics = layoutMetrics;

  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}
void CompParagraphComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  ensureVisual();
  updateVisualBrush();
}
void CompParagraphComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps CompParagraphComponentView::props() noexcept {
  assert(false);
  return {};
}

facebook::react::Tag CompParagraphComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  /*
  facebook::react::Tag tag;
  if (std::any_of(m_children.rbegin(), m_children.rend(), [&tag, &ptLocal](auto child) {
        tag = static_cast<CompBaseComponentView *>(child)->hitTest(ptLocal);
        return tag != -1;
      }))
    return tag;
    */

  if (ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }

  return -1;
}

void CompParagraphComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    // Create a sprite visual
    winrt::Windows::UI::Composition::SpriteVisual spriteVisual{nullptr};
    spriteVisual = m_compositor.CreateSpriteVisual();
    m_visual = spriteVisual;
  }
}

void CompParagraphComponentView::updateTextAlignment(
    const butter::optional<facebook::react::TextAlignment> &fbAlignment) noexcept {
  m_textLayout = nullptr;
  if (!m_textFormat)
    return;

  DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
  if (fbAlignment) {
    switch (*fbAlignment) {
      case facebook::react::TextAlignment::Center:
        alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
        break;
      case facebook::react::TextAlignment::Justified:
        alignment = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
        break;
      case facebook::react::TextAlignment::Left:
        alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        break;
      case facebook::react::TextAlignment::Right:
        alignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
        break;
      // TODO use LTR values
      case facebook::react::TextAlignment::Natural:
        alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        break;
      default:
        assert(false);
    }
  }
  m_textFormat->SetTextAlignment(alignment);
}

void CompParagraphComponentView::updateVisualBrush() noexcept {
  bool requireNewBrush{false};

  if (!m_textFormat) {
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);

    winrt::check_hresult(DWriteFactory()->CreateTextFormat(
        m_fontFamily.c_str(),
        nullptr,
        m_fontWeight,
        m_fontStyle,
        DWRITE_FONT_STRETCH_NORMAL,
        m_fontSize,
        L"en-US",
        m_textFormat.put()));

    updateTextAlignment(paragraphProps.textAttributes.alignment);
  }

  if (!m_textLayout) {
    winrt::check_hresult(DWriteFactory()->CreateTextLayout(
        m_text.c_str(),
        (uint32_t)m_text.size(),
        m_textFormat.get(),
        m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
        m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor,
        m_textLayout.put()));

    requireNewBrush = true;
  }

  if (requireNewBrush || !m_drawingSurfaceInterop) {
    // Create the surface just big enough to hold the formatted text block.
    DWRITE_TEXT_METRICS metrics;
    winrt::check_hresult(m_textLayout->GetMetrics(&metrics));
    winrt::Windows::Foundation::Size surfaceSize = {metrics.width, metrics.height};
    winrt::Windows::UI::Composition::ICompositionDrawingSurface drawingSurface;
    drawingSurface = CompositionGraphicsDevice(m_compositor)
                         .CreateDrawingSurface(
                             surfaceSize,
                             winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                             winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    // Cache the interop pointer, since that's what we always use.
    drawingSurface.as(m_drawingSurfaceInterop);

    DrawText();

    // If the rendering device is lost, the application will recreate and replace it. We then
    // own redrawing our pixels.
    if (!m_renderDeviceReplacedToken) {
      m_renderDeviceReplacedToken =
          CompositionGraphicsDevice(m_compositor)
              .RenderingDeviceReplaced([this](
                                           winrt::Windows::UI::Composition::ICompositionGraphicsDevice source,
                                           winrt::Windows::UI::Composition::IRenderingDeviceReplacedEventArgs args) {
                // Draw the text again
                DrawText();
                return S_OK;
              });
    }

    winrt::Windows::UI::Composition::ICompositionSurface surface;
    m_drawingSurfaceInterop.as(surface);
    auto surfaceBrush = m_compositor.CreateSurfaceBrush(surface);

    // The surfaceBrush's size is based on the size the text takes up, which maybe smaller than the total visual
    // So we need to align the brush within the visual to match the text alignment.
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
    float horizAlignment{0.f};
    if (paragraphProps.textAttributes.alignment) {
      switch (*paragraphProps.textAttributes.alignment) {
        case facebook::react::TextAlignment::Center:
          horizAlignment = 0.5f;
          break;
        case facebook::react::TextAlignment::Justified:
          horizAlignment = 0.5f;
          break;
        case facebook::react::TextAlignment::Left:
          horizAlignment = 0.f;
          break;
        case facebook::react::TextAlignment::Right:
          horizAlignment = 1.f;
          break;
        // TODO use LTR values
        case facebook::react::TextAlignment::Natural:
          horizAlignment = 0.f;
          break;
        default:
          assert(false);
      }
    }
    surfaceBrush.HorizontalAlignmentRatio(horizAlignment);

    m_visual.Brush(surfaceBrush);
  }

  m_requireRedraw = false;
  if (m_requireRedraw) {
    DrawText();
  }
}

// Renders the text into our composition surface
void CompParagraphComponentView::DrawText() noexcept {
  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
  POINT offset;

  if (m_text.empty()) {
    return;
  }

  if (CheckForDeviceRemoved(m_drawingSurfaceInterop->BeginDraw(
          nullptr, __uuidof(ID2D1DeviceContext), d2dDeviceContext.put_void(), &offset))) {
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);

    // Create a solid color brush for the text. A more sophisticated application might want
    // to cache and reuse a brush across all text elements instead, taking care to recreate
    // it in the event of device removed.
    winrt::com_ptr<ID2D1SolidColorBrush> brush;
    if (paragraphProps.textAttributes.foregroundColor) {
      auto winColor = paragraphProps.textAttributes.foregroundColor.AsWindowsColor();
      D2D1::ColorF color{winColor.R / 255.0f, winColor.G / 255.0f, winColor.B / 255.0f, winColor.A / 255.0f};
      winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(color, brush.put()));
    } else {
      winrt::check_hresult(
          d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), brush.put()));
    }

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

    // Draw the line of text at the specified offset, which corresponds to the top-left
    // corner of our drawing surface. Notice we don't call BeginDraw on the D2D device
    // context; this has already been done for us by the composition API.
    d2dDeviceContext->DrawTextLayout(
        D2D1::Point2F(static_cast<FLOAT>(offset.x), static_cast<FLOAT>(offset.y)), m_textLayout.get(), brush.get());

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    winrt::check_hresult(m_drawingSurfaceInterop->EndDraw());
    m_requireRedraw = false;
  }
}

const winrt::Windows::UI::Composition::Visual CompParagraphComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
