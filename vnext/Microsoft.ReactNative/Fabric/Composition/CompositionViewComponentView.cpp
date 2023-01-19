
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompositionViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <Views/FrameworkElementTransferProperties.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionContextHelper.h"
#include "CompositionHelpers.h"
#include "d2d1helper.h"

namespace Microsoft::ReactNative {

// TODO where should we store focus - a more complete focus management needs to be added in the future
static CompositionBaseComponentView *g_focusedComponent = nullptr;

CompositionBaseComponentView *GetFocusedComponent() noexcept {
  return g_focusedComponent;
}
void SetFocusedComponent(CompositionBaseComponentView *value) noexcept {
  if (g_focusedComponent == value)
    return;

  if (g_focusedComponent) {
    g_focusedComponent->onFocusLost();
  }

  if (value)
    value->onFocusGained();

  g_focusedComponent = value;
}

CompositionBaseComponentView::CompositionBaseComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : m_tag(tag), m_compContext(compContext) {}

facebook::react::Tag CompositionBaseComponentView::tag() const noexcept {
  return m_tag;
}

void CompositionBaseComponentView::parent(IComponentView *parent) noexcept {
  m_parent = parent;
}

IComponentView *CompositionBaseComponentView::parent() const noexcept {
  return m_parent;
}

void CompositionBaseComponentView::onFocusLost() noexcept {}

void CompositionBaseComponentView::onFocusGained() noexcept {}

void CompositionBaseComponentView::updateEventEmitter(
    facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  m_eventEmitter = std::static_pointer_cast<facebook::react::ViewEventEmitter const>(eventEmitter);
}

void CompositionBaseComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  assert(false); // Unhandled command
}

int64_t CompositionBaseComponentView::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  return 0;
}

RECT CompositionBaseComponentView::getClientRect() const noexcept {
  RECT rc{0};
  if (m_parent) {
    rc = m_parent->getClientRect();
  }

  rc.left += static_cast<LONG>(m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor);
  rc.top += static_cast<LONG>(m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor);
  rc.right = rc.left + static_cast<LONG>(m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor);
  rc.bottom = rc.top + static_cast<LONG>(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor);
  return rc;
}

const facebook::react::SharedViewEventEmitter &CompositionBaseComponentView::GetEventEmitter() const noexcept {
  return m_eventEmitter;
}

bool CompositionBaseComponentView::ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept {
  return false;
}

std::array<
    winrt::Microsoft::ReactNative::Composition::SpriteVisual,
    CompositionBaseComponentView::SpecialBorderLayerCount>
CompositionBaseComponentView::FindSpecialBorderLayers() const noexcept {
  std::array<winrt::Microsoft::ReactNative::Composition::SpriteVisual, SpecialBorderLayerCount> layers{
      nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

  if (m_numBorderVisuals) {
    for (uint8_t i = 0; i < m_numBorderVisuals; i++) {
      auto visual = Visual().GetAt(i);
      layers[i] = visual.as<winrt::Microsoft::ReactNative::Composition::SpriteVisual>();
    }
  }

  return layers;
}

struct RoundedPathParameters {
  float topLeftRadiusX = 0;
  float topLeftRadiusY = 0;
  float topRightRadiusX = 0;
  float topRightRadiusY = 0;
  float bottomRightRadiusX = 0;
  float bottomRightRadiusY = 0;
  float bottomLeftRadiusX = 0;
  float bottomLeftRadiusY = 0;
};

/*
 * Creates and returns a PathGeometry object used to clip the visuals of an element when a BorderRadius is set.
 * Can also be used as part of a GeometryGroup for drawing a rounded border / innerstroke when called from
 * GetGeometryForRoundedBorder. "params" defines the radii (horizontal and vertical) for each corner (top left, top
 * right, bottom right, bottom left). "rectPathGeometry" defines the bounding box of the generated shape.
 */
static winrt::com_ptr<ID2D1PathGeometry> GenerateRoundedRectPathGeometry(
    winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    const RoundedPathParameters &params,
    const facebook::react::RectangleEdges<float> &rectPathGeometry) noexcept {
  winrt::com_ptr<ID2D1PathGeometry> pathGeometry;
  winrt::com_ptr<ID2D1Factory1> spD2dFactory;
  compContext.as<Composition::ICompositionContextInterop>()->D2DFactory(spD2dFactory.put());

  // Create a path geometry.
  HRESULT hr = spD2dFactory->CreatePathGeometry(pathGeometry.put());
  if (FAILED(hr)) {
    assert(false);
    return nullptr;
  }

  // Write to the path geometry using the geometry sink.
  winrt::com_ptr<ID2D1GeometrySink> spSink = nullptr;
  hr = pathGeometry->Open(spSink.put());

  if (FAILED(hr)) {
    assert(false);
    return nullptr;
  }

  float left = rectPathGeometry.left;
  float right = rectPathGeometry.right;
  float top = rectPathGeometry.top;
  float bottom = rectPathGeometry.bottom;

  // This function uses Cubic Beziers to approximate Arc segments, even though D2D supports arcs.
  // This is INTENTIONAL. D2D Arc Segments are eventually converted into cubic beziers, but this
  // is done in such a way that we don't have control over how many bezier curve segments are used
  // for each arc. We need to ensure that we always use the same number of control points so that
  // our paths can be used in a PathKeyFrameAnimation.
  // Value for control point scale factor derived from methods described in:
  // https://web.archive.org/web/20200322075504/http://itc.ktu.lt/index.php/ITC/article/download/11812/6479
  constexpr float controlPointScaleFactor = 0.44771528244f; // 1 - (4 * (sqrtf(2.0f) - 1) / 3.0f);

#ifdef DEBUG
  // std::sqrtf is not constexpr, so we precalculated this and wrote it in a constexpr form above.
  // On debug, we should still check that the values are equivalent, though.
  static float calculatedScaleFactor = 1 - (4 * (sqrtf(2.0f) - 1) / 3.0f);
  assert(controlPointScaleFactor == calculatedScaleFactor);
#endif // DEBUG

  bool needsConsistentNumberOfControlPoints = true; // VisualVersion::IsUseWinCompClippingRegionEnabled();

  if (needsConsistentNumberOfControlPoints || (params.topLeftRadiusX != 0.0 && params.topLeftRadiusY != 0.0)) {
    spSink->BeginFigure(D2D1::Point2F(left + params.topLeftRadiusX, top), D2D1_FIGURE_BEGIN_FILLED);
  } else {
    spSink->BeginFigure(D2D1::Point2F(left, top), D2D1_FIGURE_BEGIN_FILLED);
  }

  // Move to the top right corner
  spSink->AddLine(D2D1::Point2F(right - params.topRightRadiusX, top));
  if (needsConsistentNumberOfControlPoints) {
    D2D1_BEZIER_SEGMENT arcSegmentTopRight = {
        D2D1::Point2F(right - controlPointScaleFactor * params.topRightRadiusX, top),
        D2D1::Point2F(right, top + controlPointScaleFactor * params.topRightRadiusY),
        D2D1::Point2F(right, top + params.topRightRadiusY)};

    spSink->AddBezier(&arcSegmentTopRight);
  } else if (params.topRightRadiusX != 0.0 && params.topRightRadiusY != 0.0) {
    D2D1_ARC_SEGMENT arcSegmentTopRight = {
        D2D1::Point2F(right, top + params.topRightRadiusY),
        D2D1::SizeF(params.topRightRadiusX, params.topRightRadiusY),
        0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE,
        D2D1_ARC_SIZE_SMALL};

    spSink->AddArc(&arcSegmentTopRight);
  } else {
    spSink->AddLine(D2D1::Point2F(right, top));
  }

  // Move to the bottom right corner
  spSink->AddLine(D2D1::Point2F(right, bottom - params.bottomRightRadiusY));
  if (needsConsistentNumberOfControlPoints) {
    D2D1_BEZIER_SEGMENT arcSegmentBottomRight = {
        D2D1::Point2F(right, bottom - controlPointScaleFactor * params.bottomRightRadiusY),
        D2D1::Point2F(right - controlPointScaleFactor * params.bottomRightRadiusX, bottom),
        D2D1::Point2F(right - params.bottomRightRadiusX, bottom)};

    spSink->AddBezier(&arcSegmentBottomRight);
  } else if (params.bottomRightRadiusX != 0.0 && params.bottomRightRadiusY != 0.0) {
    D2D1_ARC_SEGMENT arcSegmentBottomRight = {
        D2D1::Point2F(right - params.bottomRightRadiusX, bottom),
        D2D1::SizeF(params.bottomRightRadiusX, params.bottomRightRadiusY),
        0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE,
        D2D1_ARC_SIZE_SMALL};

    spSink->AddArc(&arcSegmentBottomRight);
  } else {
    spSink->AddLine(D2D1::Point2F(right, bottom));
  }

  // Move to the bottom left corner
  spSink->AddLine(D2D1::Point2F(left + params.bottomLeftRadiusX, bottom));
  if (needsConsistentNumberOfControlPoints) {
    D2D1_BEZIER_SEGMENT arcSegmentBottomLeft = {
        D2D1::Point2F(left + controlPointScaleFactor * params.bottomLeftRadiusX, bottom),
        D2D1::Point2F(left, bottom - controlPointScaleFactor * params.bottomLeftRadiusY),
        D2D1::Point2F(left, bottom - params.bottomLeftRadiusY)};

    spSink->AddBezier(&arcSegmentBottomLeft);
  } else if (params.bottomLeftRadiusX != 0.0 && params.bottomLeftRadiusY != 0.0) {
    D2D1_ARC_SEGMENT arcSegmentBottomLeft = {
        D2D1::Point2F(left, bottom - params.bottomLeftRadiusY),
        D2D1::SizeF(params.bottomLeftRadiusX, params.bottomLeftRadiusY),
        0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE,
        D2D1_ARC_SIZE_SMALL};

    spSink->AddArc(&arcSegmentBottomLeft);
  } else {
    spSink->AddLine(D2D1::Point2F(left, bottom));
  }

  // Move to the top left corner
  spSink->AddLine(D2D1::Point2F(left, top + params.topLeftRadiusY));
  if (needsConsistentNumberOfControlPoints) {
    D2D1_BEZIER_SEGMENT arcSegmentTopLeft = {
        D2D1::Point2F(left, top + controlPointScaleFactor * params.topLeftRadiusY),
        D2D1::Point2F(left + controlPointScaleFactor * params.topLeftRadiusX, top),
        D2D1::Point2F(left + params.topLeftRadiusX, top)};

    spSink->AddBezier(&arcSegmentTopLeft);
  } else if (params.topLeftRadiusX != 0.0 && params.topLeftRadiusY != 0.0) {
    D2D1_ARC_SEGMENT arcSegmentTopLeft = {
        D2D1::Point2F(left + params.topLeftRadiusX, top),
        D2D1::SizeF(params.topLeftRadiusX, params.topLeftRadiusY),
        0.0f,
        D2D1_SWEEP_DIRECTION_CLOCKWISE,
        D2D1_ARC_SIZE_SMALL};

    spSink->AddArc(&arcSegmentTopLeft);
  } else {
    spSink->AddLine(D2D1::Point2F(left, top));
  }

  spSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  spSink->Close();

  return pathGeometry;
}

RoundedPathParameters GenerateRoundedPathParameters(
    const facebook::react::RectangleCorners<float> &baseRadius,
    const facebook::react::RectangleEdges<float> &inset,
    const facebook::react::Size &pathSize) noexcept {
  RoundedPathParameters result;

  if (pathSize.width == 0 || pathSize.height == 0) {
    return result;
  }

  float totalTopRadius = baseRadius.topLeft + baseRadius.topRight;
  float totalRightRadius = baseRadius.topRight + baseRadius.bottomRight;
  float totalBottomRadius = baseRadius.bottomRight + baseRadius.bottomLeft;
  float totalLeftRadius = baseRadius.bottomLeft + baseRadius.topLeft;

  float maxHorizontalRadius = std::max(totalTopRadius, totalBottomRadius);
  float maxVerticalRadius = std::max(totalLeftRadius, totalRightRadius);

  double totalWidth = inset.left + inset.right + pathSize.width;
  double totalHeight = inset.top + inset.bottom + pathSize.height;

  float scaleHoriz = static_cast<float>(maxHorizontalRadius / totalWidth);
  float scaleVert = static_cast<float>(maxVerticalRadius / totalHeight);

  float maxScale = std::max(1.0f, std::max(scaleHoriz, scaleVert));

  result.topLeftRadiusX = std::max(0.0f, baseRadius.topLeft / maxScale - inset.left);
  result.topLeftRadiusY = std::max(0.0f, baseRadius.topLeft / maxScale - inset.top);
  result.topRightRadiusX = std::max(0.0f, baseRadius.topRight / maxScale - inset.right);
  result.topRightRadiusY = std::max(0.0f, baseRadius.topRight / maxScale - inset.top);
  result.bottomRightRadiusX = std::max(0.0f, baseRadius.bottomRight / maxScale - inset.right);
  result.bottomRightRadiusY = std::max(0.0f, baseRadius.bottomRight / maxScale - inset.bottom);
  result.bottomLeftRadiusX = std::max(0.0f, baseRadius.bottomLeft / maxScale - inset.left);
  result.bottomLeftRadiusY = std::max(0.0f, baseRadius.bottomLeft / maxScale - inset.bottom);

  return result;
}

static winrt::com_ptr<ID2D1PathGeometry> GenerateRoundedRectPathGeometry(
    winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    const facebook::react::RectangleCorners<float> &baseRadius,
    const facebook::react::RectangleEdges<float> &inset,
    const facebook::react::RectangleEdges<float> &rectPathGeometry) noexcept {
  RoundedPathParameters params = GenerateRoundedPathParameters(
      baseRadius,
      inset,
      {rectPathGeometry.right - rectPathGeometry.left, rectPathGeometry.bottom - rectPathGeometry.top});

  return GenerateRoundedRectPathGeometry(compContext, params, rectPathGeometry);
}

void DrawShape(
    ID2D1RenderTarget *pRT,
    const D2D1_RECT_F &rect,
    ID2D1Brush *brush,
    FLOAT strokeWidth,
    ID2D1StrokeStyle *strokeStyle) {
  pRT->DrawRectangle(rect, brush, strokeWidth, strokeStyle);
}

void DrawShape(ID2D1RenderTarget *pRT, ID2D1GeometryGroup &geometry, ID2D1Brush *brush, FLOAT, ID2D1StrokeStyle *) {
  pRT->FillGeometry(&geometry, brush);
}

void DrawShape(
    ID2D1RenderTarget *pRT,
    ID2D1PathGeometry &geometry,
    ID2D1Brush *brush,
    FLOAT strokeWidth,
    ID2D1StrokeStyle *strokeStyle) {
  pRT->DrawGeometry(&geometry, brush, strokeWidth, strokeStyle);
}

struct AutoDrawHelper {
  AutoDrawHelper(winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> &surface) {
    m_surface = surface;
    m_surface->BeginDraw(m_pRT.put(), &m_offset);
  }

  ~AutoDrawHelper() {
    m_surface->EndDraw();
  }

  const winrt::com_ptr<ID2D1DeviceContext> &GetRenderTarget() const noexcept {
    return m_pRT;
  }

  POINT Offset() const noexcept {
    return m_offset;
  }

 private:
  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> m_surface;
  POINT m_offset;
  winrt::com_ptr<ID2D1DeviceContext> m_pRT;
};

template <typename TShape>
void SetBorderLayerPropertiesCommon(
    winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    winrt::Microsoft::ReactNative::Composition::SpriteVisual &layer,
    TShape &shape,
    winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> &borderTexture,
    const D2D1_RECT_F &textureRect,
    facebook::react::Point anchorPoint,
    facebook::react::Point anchorOffset,
    winrt::Windows::Foundation::Numerics::float2 size,
    winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment,
    FLOAT strokeWidth,
    const facebook::react::SharedColor &borderColor,
    facebook::react::BorderStyle borderStyle) {
  if ((textureRect.right - textureRect.left) <= 0 && (textureRect.bottom - textureRect.top) <= 0)
    return;

  auto surface = compContext.CreateDrawingSurface(
      {(textureRect.right - textureRect.left), (textureRect.bottom - textureRect.top)},
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
  surface.as(borderTexture);

  layer.Brush(compContext.CreateSurfaceBrush(surface));
  layer.Offset({anchorOffset.x, anchorOffset.y, 0}, {anchorPoint.x, anchorPoint.y, 0});
  layer.RelativeSizeWithOffset(size, relativeSizeAdjustment);

  AutoDrawHelper autoDraw(borderTexture);

  winrt::com_ptr<ID2D1DeviceContext> pRT{autoDraw.GetRenderTarget()};

  if (!pRT) {
    return;
  }

  // Clear with transparency
  pRT->Clear();

  if (!facebook::react::isColorMeaningful(borderColor)) {
    return;
  }

  winrt::com_ptr<ID2D1Factory> spFactory;
  pRT->GetFactory(spFactory.put());
  assert(spFactory);
  if (spFactory == nullptr)
    return;

  winrt::com_ptr<ID2D1SolidColorBrush> spBorderBrush;
  pRT->CreateSolidColorBrush(borderColor.AsD2DColor(), spBorderBrush.put());
  assert(spBorderBrush);
  if (spBorderBrush == nullptr)
    return;

  winrt::com_ptr<ID2D1StrokeStyle> spStrokeStyle;

  enum class BorderStyle { Solid, Dotted, Dashed };

  if (borderStyle == facebook::react::BorderStyle::Dotted || borderStyle == facebook::react::BorderStyle::Dashed) {
    const auto capStyle =
        borderStyle == facebook::react::BorderStyle::Dashed ? D2D1_CAP_STYLE_FLAT : D2D1_CAP_STYLE_ROUND;
    const auto strokeStyleProps = D2D1::StrokeStyleProperties(
        capStyle,
        capStyle,
        capStyle,
        D2D1_LINE_JOIN_MITER,
        10.0f,
        borderStyle == facebook::react::BorderStyle::Dashed ? D2D1_DASH_STYLE_DASH : D2D1_DASH_STYLE_DOT,
        0.0f);
    spFactory->CreateStrokeStyle(&strokeStyleProps, nullptr, 0, spStrokeStyle.put());
  }
  D2D1::Matrix3x2F originalTransform;
  D2D1::Matrix3x2F translationTransform =
      D2D1::Matrix3x2F::Translation(-textureRect.left + autoDraw.Offset().x, -textureRect.top + autoDraw.Offset().y);

  pRT->GetTransform(&originalTransform);
  translationTransform = originalTransform * translationTransform;

  float oldDpiX, oldDpiY;
  pRT->SetTransform(translationTransform);
  pRT->GetDpi(&oldDpiX, &oldDpiY);
  pRT->SetDpi(96.0f, 96.0f); // We have already done the dpi scaling...

  DrawShape(pRT.get(), shape, spBorderBrush.get(), strokeWidth, spStrokeStyle.get());

  pRT->SetDpi(oldDpiX, oldDpiY);
  pRT->SetTransform(originalTransform);
}

template <typename TShape>
void SetBorderLayerProperties(
    winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    winrt::Microsoft::ReactNative::Composition::SpriteVisual &layer,
    TShape &shape,
    winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> &borderTexture,
    const D2D1_RECT_F &textureRect,
    facebook::react::Point anchorPoint,
    facebook::react::Point anchorOffset,
    winrt::Windows::Foundation::Numerics::float2 size,
    winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment,
    FLOAT strokeWidth,
    const facebook::react::SharedColor &borderColor,
    facebook::react::BorderStyle borderStyle) {
  if constexpr (!std::is_base_of_v<ID2D1GeometryGroup, TShape>) {
    SetBorderLayerPropertiesCommon(
        compContext,
        layer,
        shape,
        borderTexture,
        textureRect,
        anchorPoint,
        anchorOffset,
        size,
        relativeSizeAdjustment,
        strokeWidth,
        borderColor,
        borderStyle);
  } else {
    // if (VisualVersion::IsUseWinCompClippingRegionEnabled())
    {
      layer.Offset({anchorOffset.x, anchorOffset.y, 0}, {anchorPoint.x, anchorPoint.y, 0});
      layer.Size({textureRect.right - textureRect.left, textureRect.bottom - textureRect.top});

      layer.Brush(compContext.CreateColorBrush(borderColor.AsWindowsColor()));

      winrt::com_ptr<ID2D1Factory1> spD2dFactory;
      compContext.as<Composition::ICompositionContextInterop>()->D2DFactory(spD2dFactory.put());

      winrt::com_ptr<ID2D1TransformedGeometry> transformedShape;
      D2D1::Matrix3x2F translationTransform = D2D1::Matrix3x2F::Translation(-textureRect.left, -textureRect.top);
      winrt::check_hresult(
          spD2dFactory->CreateTransformedGeometry(&shape, &translationTransform, transformedShape.put()));

      layer.as<Composition::IVisualInterop>()->SetClippingPath(transformedShape.get());
    }
    /*
                else
                {
                        SetBorderLayerPropertiesCommon(comContext, layer, shape, borderTexture, textureRect,
       anchorPoint, anchorOffset, strokeWidth, borderColor, borderStyle);
                }
    */
  }
}

namespace AnchorPosition {
const float Left = 0.0;
const float Center = 0.5;
const float Right = 1.0;
const float Top = 0.0;
const float Bottom = 1.0;
} // namespace AnchorPosition

template <typename TShape>
void DrawAllBorderLayers(
    winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    std::array<
        winrt::Microsoft::ReactNative::Composition::SpriteVisual,
        CompositionBaseComponentView::SpecialBorderLayerCount> &spBorderLayers,
    TShape &shape,
    const facebook::react::BorderWidths &borderWidths,
    const facebook::react::BorderRadii &borderRadii,
    float textureWidth,
    float textureHeight,
    const facebook::react::BorderColors &borderColors,
    facebook::react::BorderStyle borderStyle) {
  // Now that we've drawn our nice border in one layer, split it into its component layers
  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop>
      spTextures[CompositionBaseComponentView::SpecialBorderLayerCount];

  // Set component border properties
  // Top Left Corner
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[0],
      shape,
      spTextures[0], // Target Layer, Source Texture, Target Texture
      {0,
       0,
       borderRadii.topLeft + borderWidths.left,
       borderRadii.topLeft + borderWidths.top}, // Texture Left, Top, Width, Height
      {AnchorPosition::Left, AnchorPosition::Top}, // Layer Anchor Point
      {0, 0}, // Layer Anchor Offset
      {borderRadii.topLeft + borderWidths.left, borderRadii.topLeft + borderWidths.top}, // size
      {0.0f, 0.0f}, // relativeSize
      std::max(borderWidths.left, borderWidths.top),
      borderColors.left ? borderColors.left : borderColors.top,
      borderStyle);

  // Top Edge Border
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[1],
      shape,
      spTextures[1],
      {borderRadii.topLeft + borderWidths.left,
       0,
       textureWidth - (borderRadii.topRight + borderWidths.right),
       borderWidths.top},
      {AnchorPosition::Left, AnchorPosition::Top},
      {borderRadii.topLeft + borderWidths.left, 0},
      {-(borderRadii.topLeft + borderWidths.left + borderRadii.topRight + borderWidths.right),
       borderWidths.top}, // size
      {1.0f, 0.0f}, // relativeSize
      borderWidths.top,
      borderColors.top,
      borderStyle);

  // Top Right Corner Border
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[2],
      shape,
      spTextures[2],
      {textureWidth - (borderRadii.topRight + borderWidths.right),
       0,
       textureWidth,
       borderRadii.topRight + borderWidths.top},
      {AnchorPosition::Right, AnchorPosition::Top},
      {-(borderRadii.topRight + borderWidths.right), 0},
      {borderRadii.topRight + borderWidths.right, borderRadii.topRight + borderWidths.top},
      {0.0f, 0.0f},
      std::max(borderWidths.right, borderWidths.top),
      borderColors.right ? borderColors.right : borderColors.top,
      borderStyle);

  // Right Edge Border
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[3],
      shape,
      spTextures[3],
      {textureWidth - borderWidths.right,
       borderWidths.top + borderRadii.topRight,
       textureWidth,
       textureHeight - (borderWidths.bottom + borderRadii.bottomRight)},
      {AnchorPosition::Right, AnchorPosition::Top},
      {-borderWidths.right, borderWidths.top + borderRadii.topRight},
      {borderWidths.right,
       -(borderWidths.top + borderRadii.topRight + borderWidths.bottom + borderRadii.bottomRight)}, // size
      {0.0f, 1.0f},
      borderWidths.right,
      borderColors.right,
      borderStyle);

  // Bottom Right Corner Border
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[4],
      shape,
      spTextures[4],
      {textureWidth - (borderWidths.right + borderRadii.bottomRight),
       textureHeight - (borderWidths.bottom + borderRadii.bottomRight),
       textureWidth,
       textureHeight},
      {AnchorPosition::Right, AnchorPosition::Bottom},
      {-(borderWidths.right + borderRadii.bottomRight), -(borderWidths.bottom + borderRadii.bottomRight)},
      {borderWidths.right + borderRadii.bottomRight, borderWidths.bottom + borderRadii.bottomRight},
      {0, 0},
      std::max(borderWidths.right, borderWidths.bottom),
      borderColors.right ? borderColors.right : borderColors.bottom,
      borderStyle);

  // Bottom Edge Border
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[5],
      shape,
      spTextures[5],
      {borderWidths.left + borderRadii.bottomLeft,
       textureHeight - borderWidths.bottom,
       textureWidth - (borderWidths.right + borderRadii.bottomLeft),
       textureHeight},
      {AnchorPosition::Left, AnchorPosition::Bottom},
      {borderWidths.left + borderRadii.bottomLeft, -borderWidths.bottom},
      {-(borderWidths.right + borderRadii.bottomLeft + borderWidths.left + borderRadii.bottomLeft),
       borderWidths.bottom},
      {1.0f, 0.0f},
      borderWidths.bottom,
      borderColors.bottom,
      borderStyle);

  // Bottom Left Corner Border
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[6],
      shape,
      spTextures[6],
      {0,
       textureHeight - (borderWidths.bottom + borderRadii.bottomLeft),
       borderWidths.left + borderRadii.bottomLeft,
       textureHeight},
      {AnchorPosition::Left, AnchorPosition::Bottom},
      {0, -(borderWidths.bottom + borderRadii.bottomLeft)},
      {borderWidths.left + borderRadii.bottomLeft, borderWidths.bottom + borderRadii.bottomLeft},
      {0, 0},
      std::max(borderWidths.left, borderWidths.bottom),
      borderColors.left ? borderColors.left : borderColors.bottom,
      borderStyle);

  // Left Edge Border
  SetBorderLayerProperties(
      compContext,
      spBorderLayers[7],
      shape,
      spTextures[7],
      {0,
       borderWidths.top + borderRadii.topLeft,
       borderWidths.left,
       textureHeight - (borderWidths.bottom + borderRadii.bottomLeft)},
      {AnchorPosition::Left, AnchorPosition::Top},
      {0, borderWidths.top + borderRadii.topLeft},
      {borderWidths.left, -(borderWidths.top + borderRadii.topLeft + borderWidths.bottom + borderRadii.bottomLeft)},
      {0, 1},
      borderWidths.left,
      borderColors.left,
      borderStyle);
}

winrt::com_ptr<ID2D1GeometryGroup> GetGeometryForRoundedBorder(
    winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    const facebook::react::RectangleCorners<float> &radius,
    const facebook::react::RectangleEdges<float> &inset,
    const facebook::react::RectangleEdges<float> &thickness,
    const facebook::react::RectangleEdges<float> &rectPathGeometry) noexcept {
  winrt::com_ptr<ID2D1PathGeometry> outerPathGeometry =
      GenerateRoundedRectPathGeometry(compContext, radius, inset, rectPathGeometry);

  if (outerPathGeometry == nullptr) {
    assert(false);
    return nullptr;
  }

  facebook::react::RectangleEdges<float> rectInnerPathGeometry = {
      rectPathGeometry.left + thickness.left,
      rectPathGeometry.top + thickness.top,
      rectPathGeometry.right - thickness.right,
      rectPathGeometry.bottom - thickness.bottom};

  // Total thickness is larger than original element size.
  // Clamp inner rect to have a width/height of 0, but placed such that the ratio of side-thicknesses is respected.
  // We need to respect this ratio so that any animations work properly.

  if (rectInnerPathGeometry.left > rectInnerPathGeometry.right) {
    float leftRatio = thickness.left / (thickness.left + thickness.right);
    auto x = std::floor(rectPathGeometry.left + ((rectPathGeometry.right - rectPathGeometry.left) * leftRatio));
    rectInnerPathGeometry.left = x;
    rectInnerPathGeometry.right = x;
  }

  if (rectInnerPathGeometry.top > rectInnerPathGeometry.bottom) {
    float topRatio = thickness.top / (thickness.top + thickness.bottom);
    auto y = rectPathGeometry.top + std::floor((rectPathGeometry.top - rectPathGeometry.bottom) * topRatio);
    rectInnerPathGeometry.top = y;
    rectInnerPathGeometry.bottom = y;
  }

  facebook::react::RectangleEdges<float> innerInset = {
      inset.left + thickness.left,
      inset.top + thickness.top,
      inset.right + thickness.right,
      inset.bottom + thickness.bottom};

  winrt::com_ptr<ID2D1PathGeometry> innerPathGeometry =
      GenerateRoundedRectPathGeometry(compContext, radius, innerInset, rectInnerPathGeometry);

  if (innerPathGeometry == nullptr) {
    assert(false); // Failed to create inner pathGeometry for rounded border
    return nullptr;
  }

  ID2D1Geometry *ppGeometries[] = {outerPathGeometry.get(), innerPathGeometry.get()};
  winrt::com_ptr<ID2D1Factory1> spD2dFactory;
  compContext.as<Composition::ICompositionContextInterop>()->D2DFactory(spD2dFactory.put());

  winrt::com_ptr<ID2D1GeometryGroup> geometryGroup = nullptr;
  // Create a geometry group.
  HRESULT hr = spD2dFactory->CreateGeometryGroup(
      D2D1_FILL_MODE_ALTERNATE, ppGeometries, ARRAYSIZE(ppGeometries), geometryGroup.put());

  if (SUCCEEDED(hr)) {
    return geometryGroup;
  }
  return nullptr;
}

// We dont want half pixel borders, or border radii - they lead to blurry borders
// Also apply scale factor to the radii at this point
void pixelRoundBorderRadii(facebook::react::BorderRadii &borderRadii, float scaleFactor) noexcept {
  // Always round radii down to avoid spikey circles
  borderRadii.topLeft = std::floor(borderRadii.topLeft * scaleFactor);
  borderRadii.topRight = std::floor(borderRadii.topRight * scaleFactor);
  borderRadii.bottomLeft = std::floor(borderRadii.bottomLeft * scaleFactor);
  borderRadii.bottomRight = std::floor(borderRadii.bottomRight * scaleFactor);
}

void scaleAndPixelRoundBorderWidths(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::BorderMetrics &borderMetrics,
    float scaleFactor) noexcept {
  borderMetrics.borderWidths.left = (borderMetrics.borderWidths.left == 0)
      ? 0.f
      : std::max(1.f, std::round(borderMetrics.borderWidths.left * scaleFactor));
  borderMetrics.borderWidths.top = (borderMetrics.borderWidths.top == 0)
      ? 0.f
      : std::max(1.f, std::round(borderMetrics.borderWidths.top * scaleFactor));
  borderMetrics.borderWidths.right = (borderMetrics.borderWidths.right == 0)
      ? 0.f
      : std::max(1.f, std::round(borderMetrics.borderWidths.right * scaleFactor));
  borderMetrics.borderWidths.bottom = (borderMetrics.borderWidths.bottom == 0)
      ? 0.f
      : std::max(1.f, std::round(borderMetrics.borderWidths.bottom * scaleFactor));

  // If we rounded both sides of the borderWidths up, we may have made the borderWidths larger than the total
  if (layoutMetrics.frame.size.width * scaleFactor <
      (borderMetrics.borderWidths.left + borderMetrics.borderWidths.right)) {
    borderMetrics.borderWidths.right--;
  }
  if (layoutMetrics.frame.size.height * scaleFactor <
      (borderMetrics.borderWidths.top + borderMetrics.borderWidths.bottom)) {
    borderMetrics.borderWidths.bottom--;
  }
}

// react-native uses black as a default color when none is specified.
void assignDefaultBlackBorders(facebook::react::BorderMetrics &borderMetrics) noexcept {
  if (!borderMetrics.borderColors.left) {
    borderMetrics.borderColors.left = facebook::react::blackColor();
  }
  if (!borderMetrics.borderColors.top) {
    borderMetrics.borderColors.top = facebook::react::blackColor();
  }
  if (!borderMetrics.borderColors.right) {
    borderMetrics.borderColors.right = facebook::react::blackColor();
  }
  if (!borderMetrics.borderColors.bottom) {
    borderMetrics.borderColors.bottom = facebook::react::blackColor();
  }
}

facebook::react::BorderMetrics resolveAndAlignBorderMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = viewProps.resolveBorderMetrics(layoutMetrics);

  pixelRoundBorderRadii(borderMetrics.borderRadii, layoutMetrics.pointScaleFactor);
  scaleAndPixelRoundBorderWidths(layoutMetrics, borderMetrics, layoutMetrics.pointScaleFactor);
  assignDefaultBlackBorders(borderMetrics);
  return borderMetrics;
}

bool CompositionBaseComponentView::TryUpdateSpecialBorderLayers(
    std::array<winrt::Microsoft::ReactNative::Composition::SpriteVisual, SpecialBorderLayerCount> &spBorderVisuals,
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = resolveAndAlignBorderMetrics(layoutMetrics, viewProps);
  // We only handle a single borderStyle for now
  auto borderStyle = borderMetrics.borderStyles.left;

  if (borderMetrics.borderColors.isUniform()) {
    if (!borderMetrics.borderWidths.left)
      return false;
    if (!facebook::react::isColorMeaningful(borderMetrics.borderColors.left))
      return false;
  }

  // Create the special border layers if they don't exist yet
  if (!spBorderVisuals[0]) {
    for (uint8_t i = 0; i < SpecialBorderLayerCount; i++) {
      auto visual = m_compContext.CreateSpriteVisual();
      Visual().InsertAt(visual, i);
      spBorderVisuals[i] = std::move(visual);
      m_numBorderVisuals++;
    }
  }

  float extentWidth = layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor;
  float extentHeight = layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor;

  if (borderMetrics.borderRadii.topLeft != 0 || borderMetrics.borderRadii.topRight != 0 ||
      borderMetrics.borderRadii.bottomLeft != 0 || borderMetrics.borderRadii.bottomRight != 0) {
    if (borderStyle == facebook::react::BorderStyle::Dotted || borderStyle == facebook::react::BorderStyle::Dashed) {
      // Because in DirectX geometry starts at the center of the stroke, we need to deflate
      // rectangle by half the stroke width to render correctly.
      facebook::react::RectangleEdges<float> rectPathGeometry = {
          borderMetrics.borderWidths.left / 2.0f,
          borderMetrics.borderWidths.top / 2.0f,
          extentWidth - borderMetrics.borderWidths.right / 2.0f,
          extentHeight - borderMetrics.borderWidths.bottom / 2.0f};

      winrt::com_ptr<ID2D1PathGeometry> pathGeometry =
          GenerateRoundedRectPathGeometry(m_compContext, borderMetrics.borderRadii, {0, 0, 0, 0}, rectPathGeometry);

      if (pathGeometry) {
        DrawAllBorderLayers(
            m_compContext,
            spBorderVisuals,
            *pathGeometry,
            borderMetrics.borderWidths,
            borderMetrics.borderRadii,
            extentWidth,
            extentHeight,
            borderMetrics.borderColors,
            borderStyle);
      } else {
        assert(false);
      }
    } else {
      facebook::react::RectangleEdges<float> rectPathGeometry = {0, 0, extentWidth, extentHeight};

      winrt::com_ptr<ID2D1GeometryGroup> pathGeometry = GetGeometryForRoundedBorder(
          m_compContext,
          borderMetrics.borderRadii,
          {0, 0, 0, 0}, // inset
          borderMetrics.borderWidths,
          rectPathGeometry);

      DrawAllBorderLayers(
          m_compContext,
          spBorderVisuals,
          *pathGeometry,
          borderMetrics.borderWidths,
          borderMetrics.borderRadii,
          extentWidth,
          extentHeight,
          borderMetrics.borderColors,
          borderStyle);
    }
  } else {
    // Because in DirectX geometry starts at the center of the stroke, we need to deflate rectangle by half the stroke
    // width / height to render correctly.
    D2D1_RECT_F rectShape{
        borderMetrics.borderWidths.left / 2.0f,
        borderMetrics.borderWidths.top / 2.0f,
        extentWidth - (borderMetrics.borderWidths.right / 2.0f),
        extentHeight - (borderMetrics.borderWidths.bottom / 2.0f)};
    DrawAllBorderLayers(
        m_compContext,
        spBorderVisuals,
        rectShape,
        borderMetrics.borderWidths,
        borderMetrics.borderRadii,
        extentWidth,
        extentHeight,
        borderMetrics.borderColors,
        borderStyle);
  }
  return true;
}

void CompositionBaseComponentView::UpdateSpecialBorderLayers(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto spBorderLayers = FindSpecialBorderLayers();

  if (!TryUpdateSpecialBorderLayers(spBorderLayers, layoutMetrics, viewProps)) {
    for (auto &spBorderLayer : spBorderLayers) {
      if (spBorderLayer) {
        spBorderLayer.as<winrt::Microsoft::ReactNative::Composition::SpriteVisual>().Brush(nullptr);
      }
    }
  }
}

void CompositionBaseComponentView::updateBorderProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  if (oldViewProps.borderColors != newViewProps.borderColors || oldViewProps.borderRadii != newViewProps.borderRadii ||
      !(oldViewProps.yogaStyle.border() == newViewProps.yogaStyle.border()) ||
      oldViewProps.borderStyles != newViewProps.borderStyles) {
    m_needsBorderUpdate = true;
  }
}

void CompositionBaseComponentView::updateBorderLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = resolveAndAlignBorderMetrics(layoutMetrics, viewProps);

  if (borderMetrics.borderRadii.topLeft == 0 && borderMetrics.borderRadii.topRight == 0 &&
      borderMetrics.borderRadii.bottomLeft == 0 && borderMetrics.borderRadii.bottomRight == 0) {
    Visual().as<Composition::IVisualInterop>()->SetClippingPath(nullptr);
  } else {
    winrt::com_ptr<ID2D1PathGeometry> pathGeometry = GenerateRoundedRectPathGeometry(
        m_compContext,
        borderMetrics.borderRadii,
        {0, 0, 0, 0},
        {0,
         0,
         layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
         layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});

    Visual().as<Composition::IVisualInterop>()->SetClippingPath(pathGeometry.get());
  }

  if (m_needsBorderUpdate || m_layoutMetrics != layoutMetrics) {
    m_needsBorderUpdate = false;
    UpdateSpecialBorderLayers(layoutMetrics, viewProps);
  }
}

void CompositionBaseComponentView::indexOffsetForBorder(uint32_t &index) const noexcept {
  index += m_numBorderVisuals;
}

void CompositionBaseComponentView::OnRenderingDeviceLost() noexcept {}

comp::CompositionPropertySet CompositionBaseComponentView::EnsureCenterPointPropertySet() noexcept {
  if (m_centerPropSet == nullptr) {
    auto compositor =
        winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::InnerCompositor(
            m_compContext);

    m_centerPropSet = compositor.CreatePropertySet();
    UpdateCenterPropertySet();
    m_centerPropSet.InsertMatrix4x4(L"transform", winrt::Windows::Foundation::Numerics::float4x4::identity());
    m_centerPropSet.InsertVector3(L"translation", {0, 0, 0});
  }

  return m_centerPropSet;
}

// Create a PropertySet that will hold properties used for animation facades:
//   "center":      This is the center of the Visual, which is used by animations and transforms to rotate around the
//                  correct center
//   "dpiScale3":   This is a vector3 containing the scale factor for this view, used in expressions to convert from
//                  dips to px
//   "dpiScale3":   This is a vector3 containing the inverse scale factor for this view, used in expressions to
//                  convert from px to dips "translation": An additional translation to apply to the to the visual in
//                  addition to Offset.  This is used to apply any translation properties set, or animated
//   "transform":   A transform to apply to the visual.  This transform is in dips.
void CompositionBaseComponentView::UpdateCenterPropertySet() noexcept {
  if (m_centerPropSet != nullptr) {
    m_centerPropSet.InsertVector3(
        L"center",
        {m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor / 2,
         m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor / 2,
         0});
    m_centerPropSet.InsertVector3(
        L"dpiScale3",
        {m_layoutMetrics.pointScaleFactor, m_layoutMetrics.pointScaleFactor, m_layoutMetrics.pointScaleFactor});
    m_centerPropSet.InsertVector3(
        L"dpiScale3Inv",
        {1 / m_layoutMetrics.pointScaleFactor,
         1 / m_layoutMetrics.pointScaleFactor,
         1 / m_layoutMetrics.pointScaleFactor});
  }
}

void CompositionBaseComponentView::EnsureTransformMatrixFacade() noexcept {
  if (m_hasTransformMatrixFacade)
    return;
  m_hasTransformMatrixFacade = true;

  auto centerPointPropSet = EnsureCenterPointPropertySet();
  // TODO cache expression instead of creating new ones all the time
  auto expression =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::InnerCompositor(
          m_compContext)
          .CreateExpressionAnimation(
              L"Matrix4x4.CreateFromScale(PS.dpiScale3Inv) * Matrix4x4.CreateFromTranslation(PS.translation) * PS.transform * Matrix4x4.CreateFromScale(PS.dpiScale3)");
  expression.SetReferenceParameter(L"PS", centerPointPropSet);
  winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::InnerVisual(Visual())
      .StartAnimation(L"TransformMatrix", expression);
}

CompositionViewComponentView::CompositionViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {
  static auto const defaultProps = std::make_shared<facebook::react::ViewProps const>();
  m_props = defaultProps;
  m_visual = m_compContext.CreateSpriteVisual();
}

std::vector<facebook::react::ComponentDescriptorProvider>
CompositionViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void CompositionViewComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  m_children.insert(std::next(m_children.begin(), index), &childComponentView);

  indexOffsetForBorder(index);

  const_cast<IComponentView &>(childComponentView).parent(this);

  m_visual.InsertAt(static_cast<const CompositionBaseComponentView &>(childComponentView).Visual(), index);
}

void CompositionViewComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  m_children.erase(std::next(m_children.begin(), index));

  indexOffsetForBorder(index);

  const_cast<IComponentView &>(childComponentView).parent(nullptr);
  m_visual.Remove(static_cast<const CompositionBaseComponentView &>(childComponentView).Visual());
}

void CompositionViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    if (newViewProps.backgroundColor) {
      m_visual.Brush(m_compContext.CreateColorBrush((*newViewProps.backgroundColor).m_color));
    } else {
      m_visual.Brush(nullptr);
    }
  }

  if (oldViewProps.opacity != newViewProps.opacity) {
    m_visual.Opacity(newViewProps.opacity);
  }

  updateBorderProps(oldViewProps, newViewProps);

  // Shadow
  if (oldViewProps.shadowOffset != newViewProps.shadowOffset || oldViewProps.shadowColor != newViewProps.shadowColor ||
      oldViewProps.shadowOpacity != newViewProps.shadowOpacity ||
      oldViewProps.shadowRadius != newViewProps.shadowRadius) {
    auto shadow = m_compContext.CreateDropShadow();
    shadow.Offset({newViewProps.shadowOffset.width, newViewProps.shadowOffset.height, 0});
    shadow.Opacity(newViewProps.shadowOpacity);
    shadow.BlurRadius(newViewProps.shadowRadius);
    if (newViewProps.shadowColor)
      shadow.Color(newViewProps.shadowColor.AsWindowsColor());
    m_visual.Shadow(shadow);
  }

  // Transform - TODO doesn't handle multiple of the same kind of transform -- Doesn't handle hittesting updates
  if (oldViewProps.transform != newViewProps.transform) {
    winrt::Windows::Foundation::Numerics::float4x4 transformMatrix;
    transformMatrix.m11 = newViewProps.transform.matrix[0];
    transformMatrix.m12 = newViewProps.transform.matrix[1];
    transformMatrix.m13 = newViewProps.transform.matrix[2];
    transformMatrix.m14 = newViewProps.transform.matrix[3];
    transformMatrix.m21 = newViewProps.transform.matrix[4];
    transformMatrix.m22 = newViewProps.transform.matrix[5];
    transformMatrix.m23 = newViewProps.transform.matrix[6];
    transformMatrix.m24 = newViewProps.transform.matrix[7];
    transformMatrix.m31 = newViewProps.transform.matrix[8];
    transformMatrix.m32 = newViewProps.transform.matrix[9];
    transformMatrix.m33 = newViewProps.transform.matrix[10];
    transformMatrix.m34 = newViewProps.transform.matrix[11];
    transformMatrix.m41 = newViewProps.transform.matrix[12];
    transformMatrix.m42 = newViewProps.transform.matrix[13];
    transformMatrix.m43 = newViewProps.transform.matrix[14];
    transformMatrix.m44 = newViewProps.transform.matrix[15];

    auto centerPointPropSet = EnsureCenterPointPropertySet();
    centerPointPropSet.InsertMatrix4x4(L"transform", transformMatrix);

    EnsureTransformMatrixFacade();
  }

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

facebook::react::Tag CompositionViewComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag targetTag;

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
      std::any_of(m_children.rbegin(), m_children.rend(), [&targetTag, &ptLocal, &localPt](auto child) {
        targetTag = static_cast<const CompositionBaseComponentView *>(child)->hitTest(ptLocal, localPt);
        return targetTag != -1;
      }))
    return targetTag;

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return tag();
  }

  return -1;
}

facebook::react::SharedTouchEventEmitter CompositionViewComponentView::touchEventEmitter() noexcept {
  return m_eventEmitter;
}

bool CompositionViewComponentView::ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag tag;
  if (std::any_of(m_children.rbegin(), m_children.rend(), [ptLocal, delta](auto child) {
        return const_cast<CompositionBaseComponentView *>(static_cast<const CompositionBaseComponentView *>(child))
            ->ScrollWheel(ptLocal, delta);
      }))
    return true;

  return false;
}

void CompositionViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void CompositionViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  updateBorderLayoutMetrics(layoutMetrics, *m_props);

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

void CompositionViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}

void CompositionViewComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared CompositionViewComponentView::props() noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::Composition::IVisual CompositionViewComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
