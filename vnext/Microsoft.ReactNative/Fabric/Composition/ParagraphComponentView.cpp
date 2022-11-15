
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ParagraphComponentView.h"

#include <Utils/ValueUtils.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <unicode.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include "CompositionHelpers.h"

namespace Microsoft::ReactNative {

ParagraphComponentView::ParagraphComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {
  static auto const defaultProps = std::make_shared<facebook::react::ParagraphProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
ParagraphComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ParagraphComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  auto v = static_cast<const ParagraphComponentView &>(childComponentView);
  assert(false);
  // m_element.Children().InsertAt(index, v.Element());
}

void ParagraphComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  // m_element.Children().RemoveAt(index);
}

void ParagraphComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(props);

  if (oldViewProps.textAttributes.foregroundColor != newViewProps.textAttributes.foregroundColor) {
    m_requireRedraw = true;
  }
  if (oldViewProps.textAttributes.opacity != newViewProps.textAttributes.opacity) {
    m_requireRedraw = true;
  }

  if (oldViewProps.textAttributes.alignment != newViewProps.textAttributes.alignment) {
    updateTextAlignment(newViewProps.textAttributes.alignment);
  }

  m_props = std::static_pointer_cast<facebook::react::ParagraphProps const>(props);
}

void ParagraphComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}

void ParagraphComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ParagraphShadowNode::ConcreteState const>(state);

  m_attributedStringBox = facebook::react::AttributedStringBox(newState.getData().attributedString);
  m_paragraphAttributes = {}; // TODO

  m_textLayout = nullptr;
}
void ParagraphComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  ensureVisual();

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}
void ParagraphComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  ensureVisual();
  updateVisualBrush();
}
void ParagraphComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ParagraphComponentView::props() noexcept {
  assert(false);
  return {};
}

facebook::react::Tag ParagraphComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag targetTag;

  /*
    if ((m_props.pointerEvents == facebook::react::PointerEventsMode::Auto ||
        m_props.pointerEvents == facebook::react::PointerEventsMode::BoxNone) && std::any_of(m_children.rbegin(),
    m_children.rend(), [&targetTag, &ptLocal, &localPt](auto child) { targetTag = static_cast<const
    CompositionBaseComponentView
    *>(child)->hitTest(ptLocal, localPt); return targetTag != -1;
        }))
      return targetTag;
      */

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return tag();
  }

  return -1;
}

facebook::react::SharedTouchEventEmitter ParagraphComponentView::touchEventEmitter() noexcept {
  return m_eventEmitter;
}

void ParagraphComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
  }
}

void ParagraphComponentView::updateTextAlignment(
    const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept {
  m_textLayout = nullptr;
  if (!m_textLayout)
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
  // TODO
  // m_textFormat->SetTextAlignment(alignment);
}

void ParagraphComponentView::OnRenderingDeviceLost() noexcept {
  DrawText();
}

void ParagraphComponentView::updateVisualBrush() noexcept {
  bool requireNewBrush{false};

  // TODO
  // updateTextAlignment(paragraphProps.textAttributes.alignment);

  if (!m_textLayout) {
    facebook::react::LayoutConstraints contraints;
    contraints.maximumSize.width =
        m_layoutMetrics.frame.size.width - m_layoutMetrics.contentInsets.left - m_layoutMetrics.contentInsets.right;
    contraints.maximumSize.height =
        m_layoutMetrics.frame.size.height - m_layoutMetrics.contentInsets.top - m_layoutMetrics.contentInsets.bottom;

    // TODO Figure out how to get text alignment not through m_props and only use StringBox and ParagraphAttributes
    // instead
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
    const std::optional<facebook::react::TextAlignment> &textAlignment = m_props->textAttributes.alignment;

    facebook::react::TextLayoutManager::GetTextLayout(
        m_attributedStringBox, {} /*TODO*/, contraints, textAlignment, m_textLayout);
    requireNewBrush = true;
  }

  if (requireNewBrush || !m_drawingSurface) {
    if (!m_textLayout) { // Empty Text element
      m_drawingSurface = nullptr;
    } else {
      // Create the surface just big enough to hold the formatted text block.
      DWRITE_TEXT_METRICS metrics;
      winrt::check_hresult(m_textLayout->GetMetrics(&metrics));

      if (metrics.width == 0 || metrics.height == 0) {
        m_drawingSurface = nullptr;
        return;
      }

      winrt::Windows::Foundation::Size surfaceSize = {
          m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
          m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor};
      m_drawingSurface = m_compContext.CreateDrawingSurface(
          surfaceSize,
          winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
          winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
    }

    DrawText();

    auto surfaceBrush = m_compContext.CreateSurfaceBrush(m_drawingSurface);

    // The surfaceBrush's size is based on the size the text takes up, which maybe smaller than the total visual
    // So we need to align the brush within the visual to match the text alignment.
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
    float horizAlignment{0.f};
    /*
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
    */
    // TODO Using brush alignment to align the text makes it blury...
    surfaceBrush.HorizontalAlignmentRatio(horizAlignment);
    surfaceBrush.VerticalAlignmentRatio(0.f);
    surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    m_visual.Brush(surfaceBrush);
  }

  if (m_requireRedraw) {
    DrawText();
  }
}

// Renders the text into our composition surface
void ParagraphComponentView::DrawText() noexcept {
  if (!m_drawingSurface)
    return;

  if (m_layoutMetrics.frame.size.width == 0 || m_layoutMetrics.frame.size.height == 0) {
    m_requireRedraw = false;
    return;
  }

  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
  POINT offset;

  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> drawingSurfaceInterop;
  m_drawingSurface.as(drawingSurfaceInterop);

  if (CheckForDeviceRemoved(drawingSurfaceInterop->BeginDraw(d2dDeviceContext.put(), &offset))) {
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);

    // Create a solid color brush for the text. A more sophisticated application might want
    // to cache and reuse a brush across all text elements instead, taking care to recreate
    // it in the event of device removed.
    winrt::com_ptr<ID2D1SolidColorBrush> brush;
    if (paragraphProps.textAttributes.foregroundColor) {
      auto color = paragraphProps.textAttributes.foregroundColor.AsD2DColor();
      winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(color, brush.put()));
    } else {
      winrt::check_hresult(
          d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), brush.put()));
    }

    if (paragraphProps.textAttributes.textDecorationLineType) {
      DWRITE_TEXT_RANGE range = {0, std::numeric_limits<uint32_t>::max()};
      if (*(paragraphProps.textAttributes.textDecorationLineType) ==
              facebook::react::TextDecorationLineType::Underline ||
          *(paragraphProps.textAttributes.textDecorationLineType) ==
              facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
        m_textLayout->SetUnderline(true, range);
      } else {
        m_textLayout->SetUnderline(false, range);
      }
    }

    if (paragraphProps.textAttributes.textDecorationLineType) {
      DWRITE_TEXT_RANGE range = {0, std::numeric_limits<uint32_t>::max()};
      if (*(paragraphProps.textAttributes.textDecorationLineType) ==
              facebook::react::TextDecorationLineType::Strikethrough ||
          *(paragraphProps.textAttributes.textDecorationLineType) ==
              facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
        m_textLayout->SetStrikethrough(true, range);
      } else {
        m_textLayout->SetStrikethrough(false, range);
      }
    }

    if (!isnan(paragraphProps.opacity)) {
      brush->SetOpacity(paragraphProps.opacity);
    }

    // Create color effects for individual text fragments.
    unsigned int position = 0;
    unsigned int length = 0;
    for (auto fragment : m_attributedStringBox.getValue().getFragments()) {
      length = static_cast<UINT32>(fragment.string.length());
      DWRITE_TEXT_RANGE range = {position, length};
      if (fragment.textAttributes.foregroundColor &&
              (fragment.textAttributes.foregroundColor != paragraphProps.textAttributes.foregroundColor) ||
          !isnan(fragment.textAttributes.opacity)) {
        winrt::com_ptr<ID2D1SolidColorBrush> fragmentBrush;
        if (fragment.textAttributes.foregroundColor) {
          auto color = fragment.textAttributes.foregroundColor.AsD2DColor();
          winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(color, fragmentBrush.put()));
        } else {
          winrt::check_hresult(
              d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), fragmentBrush.put()));
        }

        if (fragment.textAttributes.textDecorationLineType) {
          if (*(fragment.textAttributes.textDecorationLineType) == facebook::react::TextDecorationLineType::Underline ||
              *(fragment.textAttributes.textDecorationLineType) ==
                  facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
            m_textLayout->SetUnderline(true, range);
          } else {
            m_textLayout->SetUnderline(false, range);
          }
        }

        if (fragment.textAttributes.textDecorationLineType) {
          if (*(fragment.textAttributes.textDecorationLineType) ==
                  facebook::react::TextDecorationLineType::Strikethrough ||
              *(fragment.textAttributes.textDecorationLineType) ==
                  facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
            m_textLayout->SetStrikethrough(true, range);
          } else {
            m_textLayout->SetStrikethrough(false, range);
          }
        }

        if (!isnan(fragment.textAttributes.opacity)) {
          fragmentBrush->SetOpacity(fragment.textAttributes.opacity);
        }
        m_textLayout->SetDrawingEffect(fragmentBrush.get(), range);
      }

      position += length;
    }

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
    assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);
    const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
    float oldDpiX, oldDpiY;
    d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
    d2dDeviceContext->SetDpi(dpi, dpi);

    // Draw the line of text at the specified offset, which corresponds to the top-left
    // corner of our drawing surface. Notice we don't call BeginDraw on the D2D device
    // context; this has already been done for us by the composition API.
    d2dDeviceContext->DrawTextLayout(
        D2D1::Point2F(
            static_cast<FLOAT>((offset.x + m_layoutMetrics.contentInsets.left) / m_layoutMetrics.pointScaleFactor),
            static_cast<FLOAT>((offset.y + m_layoutMetrics.contentInsets.top) / m_layoutMetrics.pointScaleFactor)),
        m_textLayout.get(),
        brush.get());

    // restore dpi to old state
    d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    winrt::check_hresult(drawingSurfaceInterop->EndDraw());
    m_requireRedraw = false;
  }
}

winrt::Microsoft::ReactNative::Composition::IVisual ParagraphComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
