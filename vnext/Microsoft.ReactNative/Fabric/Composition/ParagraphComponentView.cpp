
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ParagraphComponentView.h"

#include <Utils/ValueUtils.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <unicode.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include "Composition/AutoDraw.h"
#include "CompositionDynamicAutomationProvider.h"
#include "CompositionHelpers.h"

namespace Microsoft::ReactNative {

ParagraphComponentView::ParagraphComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {
  static auto const defaultProps = std::make_shared<facebook::react::ParagraphProps const>();
  m_props = defaultProps;
}

void ParagraphComponentView::mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
  // auto v = static_cast<ParagraphComponentView &>(childComponentView);
  assert(false);
  // m_element.Children().InsertAt(index, v.Element());
}

void ParagraphComponentView::unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
  // m_element.Children().RemoveAt(index);
}

void ParagraphComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(props);

  ensureVisual();

  if (oldViewProps.textAttributes.foregroundColor != newViewProps.textAttributes.foregroundColor) {
    m_requireRedraw = true;
  }
  if (oldViewProps.textAttributes.opacity != newViewProps.textAttributes.opacity) {
    m_requireRedraw = true;
  }

  if (oldViewProps.textAttributes.alignment != newViewProps.textAttributes.alignment) {
    updateTextAlignment(newViewProps.textAttributes.alignment);
  }

  // update BaseComponentView props
  updateAccessibilityProps(oldViewProps, newViewProps);
  updateShadowProps(oldViewProps, newViewProps, m_visual);
  updateTransformProps(oldViewProps, newViewProps, m_visual);
  updateBorderProps(oldViewProps, newViewProps);

  m_props = std::static_pointer_cast<facebook::react::ParagraphProps const>(props);
}

void ParagraphComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  Super::updateEventEmitter(eventEmitter);
}

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
void ParagraphComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  ensureVisual();
  updateVisualBrush();
}
void ParagraphComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ParagraphComponentView::props() noexcept {
  return m_props;
}

facebook::react::Tag ParagraphComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
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

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return tag();
  }

  return -1;
}

facebook::react::SharedViewEventEmitter ParagraphComponentView::eventEmitterAtPoint(
    facebook::react::Point pt) noexcept {
  if (m_attributedStringBox.getValue().getFragments().size()) {
    BOOL isTrailingHit = false;
    BOOL isInside = false;
    DWRITE_HIT_TEST_METRICS metrics;
    winrt::check_hresult(m_textLayout->HitTestPoint(pt.x, pt.y, &isTrailingHit, &isInside, &metrics));
    if (isInside) {
      uint32_t textPosition = metrics.textPosition;

      for (auto fragment : m_attributedStringBox.getValue().getFragments()) {
        if (textPosition < fragment.string.length()) {
          return std::static_pointer_cast<const facebook::react::ViewEventEmitter>(
              fragment.parentShadowView.eventEmitter);
        }
        textPosition -= static_cast<uint32_t>(fragment.string.length());
      }
    }
  }

  return m_eventEmitter;
}

void ParagraphComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
    OuterVisual().InsertAt(m_visual, 0);
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
    facebook::react::LayoutConstraints constraints;
    constraints.maximumSize.width =
        m_layoutMetrics.frame.size.width - m_layoutMetrics.contentInsets.left - m_layoutMetrics.contentInsets.right;
    constraints.maximumSize.height =
        m_layoutMetrics.frame.size.height - m_layoutMetrics.contentInsets.top - m_layoutMetrics.contentInsets.bottom;

    facebook::react::TextLayoutManager::GetTextLayout(m_attributedStringBox, {} /*TODO*/, constraints, m_textLayout);
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
      m_drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
          surfaceSize,
          winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
          winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
    }

    DrawText();

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
    // TODO Using brush alignment to align the text makes it blurry...
    if (m_drawingSurface) {
      m_drawingSurface.HorizontalAlignmentRatio(horizAlignment);
      m_drawingSurface.VerticalAlignmentRatio(0.f);
      m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    }
    m_visual.Brush(m_drawingSurface);
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

  POINT offset;
  {
    ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(m_drawingSurface, &offset);
    if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
      d2dDeviceContext->Clear(
          m_props->backgroundColor ? m_props->backgroundColor.AsD2DColor() : D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
      assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);
      const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
      float oldDpiX, oldDpiY;
      d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
      d2dDeviceContext->SetDpi(dpi, dpi);

      float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor);
      float offsetY = static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor);

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
        m_visual.Opacity(paragraphProps.opacity);
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
            if (*(fragment.textAttributes.textDecorationLineType) ==
                    facebook::react::TextDecorationLineType::Underline ||
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

          // DWrite doesn't handle background highlight colors, so we manually draw the background color for ranges
          if (facebook::react::isColorMeaningful(fragment.textAttributes.backgroundColor)) {
            UINT32 actualHitTestCount = 0;
            if (range.length > 0) {
              m_textLayout->HitTestTextRange(
                  range.startPosition,
                  range.length,
                  0, // x
                  0, // y
                  NULL,
                  0, // metrics count
                  &actualHitTestCount);
            }

            // Allocate enough room to return all hit-test metrics.
            std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTestCount);
            if (range.length > 0) {
              m_textLayout->HitTestTextRange(
                  range.startPosition,
                  range.length,
                  0, // x
                  0, // y
                  &hitTestMetrics[0],
                  static_cast<UINT32>(hitTestMetrics.size()),
                  &actualHitTestCount);
            }

            // Draw the selection ranges behind the text.
            if (actualHitTestCount > 0) {
              // Note that an ideal layout will return fractional values,
              // so you may see slivers between the selection ranges due
              // to the per-primitive anti-aliasing of the edges unless
              // it is disabled (better for performance anyway).
              auto oldAliasMode = d2dDeviceContext->GetAntialiasMode();
              d2dDeviceContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

              winrt::com_ptr<ID2D1SolidColorBrush> textHighlightBrush;
              winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(
                  fragment.textAttributes.backgroundColor.AsD2DColor(), textHighlightBrush.put()));

              for (size_t i = 0; i < actualHitTestCount; ++i) {
                const DWRITE_HIT_TEST_METRICS &htm = hitTestMetrics[i];

                const D2D1_RECT_F rect = {
                    std::round(htm.left + offsetX),
                    std::round(htm.top + offsetY),
                    std::round(htm.left + htm.width + offsetX),
                    std::round(htm.top + htm.height + offsetY)};

                d2dDeviceContext->FillRectangle(rect, textHighlightBrush.get());
              }
              d2dDeviceContext->SetAntialiasMode(oldAliasMode);
            }
          }
        }

        position += length;
      }

      // Draw the line of text at the specified offset, which corresponds to the top-left
      // corner of our drawing surface. Notice we don't call BeginDraw on the D2D device
      // context; this has already been done for us by the composition API.
      d2dDeviceContext->DrawTextLayout(
          D2D1::Point2F(
              static_cast<FLOAT>((offset.x + m_layoutMetrics.contentInsets.left) / m_layoutMetrics.pointScaleFactor),
              static_cast<FLOAT>((offset.y + m_layoutMetrics.contentInsets.top) / m_layoutMetrics.pointScaleFactor)),
          m_textLayout.get(),
          brush.get(),
          D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

      // restore dpi to old state
      d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);
    }
    m_requireRedraw = false;
  }
}

std::string ParagraphComponentView::DefaultControlType() const noexcept {
  return "text";
}

std::string ParagraphComponentView::DefaultAccessibleName() const noexcept {
  return m_attributedStringBox.getValue().getString();
}

winrt::Microsoft::ReactNative::Composition::IVisual ParagraphComponentView::Visual() const noexcept {
  return m_visual;
}

std::shared_ptr<ParagraphComponentView> ParagraphComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag) noexcept {
  return std::shared_ptr<ParagraphComponentView>(new ParagraphComponentView(compContext, tag));
}

} // namespace Microsoft::ReactNative
