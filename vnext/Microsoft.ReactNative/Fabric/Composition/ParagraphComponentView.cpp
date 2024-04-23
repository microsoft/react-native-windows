
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
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background,
          false) {
  static auto const defaultProps = std::make_shared<facebook::react::ParagraphProps const>();
  m_props = defaultProps;
}

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
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(props);

  ensureVisual();

  if (oldViewProps.textAttributes.foregroundColor != newViewProps.textAttributes.foregroundColor) {
    m_requireRedraw = true;
  }
  if (oldViewProps.textAttributes.opacity != newViewProps.textAttributes.opacity) {
    m_requireRedraw = true;
  }
  if (oldViewProps.testId != newViewProps.testId) {
    m_visual.Comment(winrt::to_hstring(newViewProps.testId));
  }

  if (oldViewProps.textAttributes.alignment != newViewProps.textAttributes.alignment) {
    updateTextAlignment(newViewProps.textAttributes.alignment);
  }

  // update BaseComponentView props
  updateAccessibilityProps(oldViewProps, newViewProps);
  updateTransformProps(oldViewProps, newViewProps, m_visual);
  Super::updateProps(props, oldProps);

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

  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
}
void ParagraphComponentView::FinalizeUpdates(
    winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  ensureVisual();
  updateVisualBrush();
  Super::FinalizeUpdates(updateMask);
}
void ParagraphComponentView::prepareForRecycle() noexcept {}

facebook::react::SharedViewProps ParagraphComponentView::viewProps() noexcept {
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
    ComponentView
    *>(child)->hitTest(ptLocal, localPt); return targetTag != -1;
        }))
      return targetTag;
      */

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
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
      m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
    }
    m_visual.Brush(m_drawingSurface);
  }

  if (m_requireRedraw) {
    DrawText();
  }
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
    ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(m_drawingSurface, &offset);
    if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
      d2dDeviceContext->Clear(
          m_props->backgroundColor ? theme()->D2DColor(*m_props->backgroundColor)
                                   : D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

      const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);

      RenderText(
          *d2dDeviceContext,
          *m_textLayout,
          m_attributedStringBox.getValue(),
          paragraphProps.textAttributes,
          {static_cast<float>(offset.x) + m_layoutMetrics.contentInsets.left,
           static_cast<float>(offset.y) + m_layoutMetrics.contentInsets.top},
          m_layoutMetrics.pointScaleFactor,
          *theme());

      if (!isnan(paragraphProps.opacity)) {
        m_visual.Opacity(paragraphProps.opacity);
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

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ParagraphComponentView::Visual() const noexcept {
  return m_visual;
}

winrt::Microsoft::ReactNative::ComponentView ParagraphComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ParagraphComponentView>(compContext, tag, reactContext);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
