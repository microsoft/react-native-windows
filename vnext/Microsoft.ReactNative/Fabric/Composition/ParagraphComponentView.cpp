
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ParagraphComponentView.h"

#include <AutoDraw.h>
#include <Utils/ValueUtils.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <unicode.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include "CompositionDynamicAutomationProvider.h"
#include "CompositionHelpers.h"
#include "TextDrawing.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ParagraphComponentView::ParagraphComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          ParagraphComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background) {}

void ParagraphComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void ParagraphComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

void ParagraphComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps =
      *std::static_pointer_cast<const facebook::react::ParagraphProps>(oldProps ? oldProps : viewProps());
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

  if (oldViewProps.paragraphAttributes.ellipsizeMode != newViewProps.paragraphAttributes.ellipsizeMode) {
    m_textLayout = nullptr;
  }

  if (oldViewProps.paragraphAttributes.adjustsFontSizeToFit != newViewProps.paragraphAttributes.adjustsFontSizeToFit) {
    m_textLayout = nullptr;
  }

  Super::updateProps(props, oldProps);
}

void ParagraphComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ParagraphShadowNode::ConcreteState const>(state);

  m_attributedStringBox = facebook::react::AttributedStringBox(newState.getData().attributedString);
  m_paragraphAttributes = facebook::react::ParagraphAttributes(newState.getData().paragraphAttributes);

  m_textLayout = nullptr;
}

void ParagraphComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);

  if (layoutMetrics.pointScaleFactor != oldLayoutMetrics.pointScaleFactor) {
    m_textLayout = nullptr;
  }
}

void ParagraphComponentView::FinalizeUpdates(
    winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  ensureVisual();
  updateVisualBrush();
  Super::FinalizeUpdates(updateMask);
}

facebook::react::SharedViewEventEmitter ParagraphComponentView::eventEmitterAtPoint(
    facebook::react::Point pt) noexcept {
  if (m_attributedStringBox.getValue().getFragments().size() && m_textLayout) {
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

void ParagraphComponentView::updateTextAlignment(
    const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept {
  // Reset text layout to force recreation with new alignment
  m_textLayout = nullptr;
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

    facebook::react::TextLayoutManager::GetTextLayout(
        m_attributedStringBox, m_paragraphAttributes, constraints, m_textLayout);

    // Apply text alignment after creating the text layout
    if (m_textLayout) {
      const auto &props = paragraphProps();
      DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
      if (props.textAttributes.alignment) {
        switch (*props.textAttributes.alignment) {
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
          case facebook::react::TextAlignment::Natural:
            alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
            break;
          default:
            alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
            break;
        }
      }
      winrt::check_hresult(m_textLayout->SetTextAlignment(alignment));
    }

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
    float horizAlignment{0.f};
    /*
    const auto &props = paragraphProps()
    if (props.textAttributes.alignment) {
      switch (*props.textAttributes.alignment) {
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
      m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
    }
    Visual().as<Experimental::ISpriteVisual>().Brush(m_drawingSurface);
  }

  if (m_requireRedraw) {
    DrawText();
  }
}

facebook::react::SharedViewProps ParagraphComponentView::defaultProps() noexcept {
  static auto const defaultProps = std::make_shared<facebook::react::ParagraphProps const>();
  return defaultProps;
}

const facebook::react::ParagraphProps &ParagraphComponentView::paragraphProps() const noexcept {
  return *std::static_pointer_cast<const facebook::react::ParagraphProps>(viewProps());
}

void ParagraphComponentView::onThemeChanged() noexcept {
  DrawText();
  Super::onThemeChanged();
}

// Renders the text into our composition surface
void ParagraphComponentView::DrawText() noexcept {
  if (!m_drawingSurface || theme()->IsEmpty())
    return;

  if (m_layoutMetrics.frame.size.width == 0 || m_layoutMetrics.frame.size.height == 0) {
    m_requireRedraw = false;
    return;
  }

  POINT offset;
  {
    ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(
        m_drawingSurface, m_layoutMetrics.pointScaleFactor, &offset);
    if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
      d2dDeviceContext->Clear(
          viewProps()->backgroundColor ? theme()->D2DColor(*viewProps()->backgroundColor)
                                       : D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
      const auto &props = paragraphProps();
      RenderText(
          *d2dDeviceContext,
          *m_textLayout,
          m_attributedStringBox.getValue(),
          props.textAttributes,
          {static_cast<float>(offset.x) + m_layoutMetrics.contentInsets.left,
           static_cast<float>(offset.y) + m_layoutMetrics.contentInsets.top},
          m_layoutMetrics.pointScaleFactor,
          *theme());

      if (!isnan(props.opacity)) {
        Visual().Opacity(props.opacity);
      }
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

winrt::Microsoft::ReactNative::ComponentView ParagraphComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ParagraphComponentView>(compContext, tag, reactContext);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
