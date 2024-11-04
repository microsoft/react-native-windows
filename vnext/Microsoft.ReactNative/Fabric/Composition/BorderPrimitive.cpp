
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BorderPrimitive.h"

#include <AutoDraw.h>
#include "CompositionViewComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

// Ideally isColorMeaningful would be sufficient here.  But it appears to detect platformColors as not meaningful
// https://github.com/microsoft/react-native-windows/issues/14006
bool isColorMeaningful(
    const facebook::react::SharedColor &color,
    winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme) noexcept {
  if (!color) {
    return false;
  }

  return theme->Color(*color).A > 0;
}

// We don't want half pixel borders, or border radii - they lead to blurry borders
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

facebook::react::BorderMetrics BorderPrimitive::resolveAndAlignBorderMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = viewProps.resolveBorderMetrics(layoutMetrics);

  pixelRoundBorderRadii(borderMetrics.borderRadii, layoutMetrics.pointScaleFactor);
  scaleAndPixelRoundBorderWidths(layoutMetrics, borderMetrics, layoutMetrics.pointScaleFactor);
  assignDefaultBlackBorders(borderMetrics);
  return borderMetrics;
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
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    const RoundedPathParameters &params,
    const facebook::react::RectangleEdges<float> &rectPathGeometry) noexcept {
  winrt::com_ptr<ID2D1PathGeometry> pathGeometry;
  winrt::com_ptr<ID2D1Factory1> spD2dFactory;
  compContext.as<::Microsoft::ReactNative::Composition::Experimental::ICompositionContextInterop>()->D2DFactory(
      spD2dFactory.put());

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

winrt::com_ptr<ID2D1PathGeometry> BorderPrimitive::GenerateRoundedRectPathGeometry(
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    const facebook::react::RectangleCorners<float> &baseRadius,
    const facebook::react::RectangleEdges<float> &inset,
    const facebook::react::RectangleEdges<float> &rectPathGeometry) noexcept {
  RoundedPathParameters params = GenerateRoundedPathParameters(
      baseRadius,
      inset,
      {rectPathGeometry.right - rectPathGeometry.left, rectPathGeometry.bottom - rectPathGeometry.top});

  return winrt::Microsoft::ReactNative::Composition::implementation::GenerateRoundedRectPathGeometry(
      compContext, params, rectPathGeometry);
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

template <typename TShape>
void SetBorderLayerPropertiesCommon(
    winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme,
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual &layer,
    TShape &shape,
    winrt::com_ptr<::Microsoft::ReactNative::Composition::Experimental::ICompositionDrawingSurfaceInterop>
        &borderTexture,
    const D2D1_RECT_F &textureRect,
    facebook::react::Point anchorPoint,
    facebook::react::Point anchorOffset,
    winrt::Windows::Foundation::Numerics::float2 size,
    winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment,
    FLOAT strokeWidth,
    const facebook::react::SharedColor &borderColor,
    facebook::react::BorderStyle borderStyle) {
  layer.Offset({anchorOffset.x, anchorOffset.y, 0}, {anchorPoint.x, anchorPoint.y, 0});
  layer.RelativeSizeWithOffset(size, relativeSizeAdjustment);
  layer.as<::Microsoft::ReactNative::Composition::Experimental::IVisualInterop>()->SetClippingPath(nullptr);

  if ((textureRect.right - textureRect.left) <= 0 || (textureRect.bottom - textureRect.top) <= 0)
    return;

  auto surface = compContext.CreateDrawingSurfaceBrush(
      {(textureRect.right - textureRect.left), (textureRect.bottom - textureRect.top)},
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
  surface.as(borderTexture);

  layer.Brush(surface);

  POINT offset;
  ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(
      surface, 1.0f /* We have already done the dpi scaling */, &offset);
  if (auto pRT = autoDraw.GetRenderTarget()) {
    // Clear with transparency
    pRT->Clear();

    if (!isColorMeaningful(borderColor, theme)) {
      return;
    }

    winrt::com_ptr<ID2D1Factory> spFactory;
    pRT->GetFactory(spFactory.put());
    assert(spFactory);
    if (spFactory == nullptr)
      return;

    winrt::com_ptr<ID2D1SolidColorBrush> spBorderBrush;
    pRT->CreateSolidColorBrush(theme->D2DColor(*borderColor), spBorderBrush.put());
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
        D2D1::Matrix3x2F::Translation(-textureRect.left + offset.x, -textureRect.top + offset.y);

    pRT->GetTransform(&originalTransform);
    translationTransform = originalTransform * translationTransform;

    pRT->SetTransform(translationTransform);

    DrawShape(pRT, shape, spBorderBrush.get(), strokeWidth, spStrokeStyle.get());

    pRT->SetTransform(originalTransform);
  }
}

template <typename TShape>
void SetBorderLayerProperties(
    winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme,
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual &layer,
    TShape &shape,
    winrt::com_ptr<::Microsoft::ReactNative::Composition::Experimental::ICompositionDrawingSurfaceInterop>
        &borderTexture,
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
        theme,
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
      layer.RelativeSizeWithOffset(
          {textureRect.right - textureRect.left, textureRect.bottom - textureRect.top}, {0.0f, 0.0f});

      layer.Brush(theme->Brush(*borderColor));

      winrt::com_ptr<ID2D1Factory1> spD2dFactory;
      compContext.as<::Microsoft::ReactNative::Composition::Experimental::ICompositionContextInterop>()->D2DFactory(
          spD2dFactory.put());

      winrt::com_ptr<ID2D1TransformedGeometry> transformedShape;
      D2D1::Matrix3x2F translationTransform = D2D1::Matrix3x2F::Translation(-textureRect.left, -textureRect.top);
      winrt::check_hresult(
          spD2dFactory->CreateTransformedGeometry(&shape, &translationTransform, transformedShape.put()));

      layer.as<::Microsoft::ReactNative::Composition::Experimental::IVisualInterop>()->SetClippingPath(
          transformedShape.get());
    }
    /*
                else
                {
                        SetBorderLayerPropertiesCommon(theme, comContext, layer, shape, borderTexture, textureRect,
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
    winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme,
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    std::array<
        winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual,
        BorderPrimitive::SpecialBorderLayerCount> &spBorderLayers,
    TShape &shape,
    const facebook::react::BorderWidths &borderWidths,
    const facebook::react::BorderRadii &borderRadii,
    float textureWidth,
    float textureHeight,
    const facebook::react::BorderColors &borderColors,
    facebook::react::BorderStyle borderStyle) {
  // Now that we've drawn our nice border in one layer, split it into its component layers
  winrt::com_ptr<::Microsoft::ReactNative::Composition::Experimental::ICompositionDrawingSurfaceInterop>
      spTextures[BorderPrimitive::SpecialBorderLayerCount];

  // Set component border properties
  // Top Left Corner
  SetBorderLayerProperties(
      theme,
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
      theme,
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
      theme,
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
      theme,
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
      theme,
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
      theme,
      compContext,
      spBorderLayers[5],
      shape,
      spTextures[5],
      {borderWidths.left + borderRadii.bottomLeft,
       textureHeight - borderWidths.bottom,
       textureWidth - (borderWidths.right + borderRadii.bottomRight),
       textureHeight},
      {AnchorPosition::Left, AnchorPosition::Bottom},
      {borderWidths.left + borderRadii.bottomLeft, -borderWidths.bottom},
      {-(borderWidths.right + borderRadii.bottomLeft + borderWidths.left + borderRadii.bottomRight),
       borderWidths.bottom},
      {1.0f, 0.0f},
      borderWidths.bottom,
      borderColors.bottom,
      borderStyle);

  // Bottom Left Corner Border
  SetBorderLayerProperties(
      theme,
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
      theme,
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
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    const facebook::react::RectangleCorners<float> &radius,
    const facebook::react::RectangleEdges<float> &inset,
    const facebook::react::RectangleEdges<float> &thickness,
    const facebook::react::RectangleEdges<float> &rectPathGeometry) noexcept {
  winrt::com_ptr<ID2D1PathGeometry> outerPathGeometry =
      BorderPrimitive::GenerateRoundedRectPathGeometry(compContext, radius, inset, rectPathGeometry);

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
      BorderPrimitive::GenerateRoundedRectPathGeometry(compContext, radius, innerInset, rectInnerPathGeometry);

  if (innerPathGeometry == nullptr) {
    assert(false); // Failed to create inner pathGeometry for rounded border
    return nullptr;
  }

  ID2D1Geometry *ppGeometries[] = {outerPathGeometry.get(), innerPathGeometry.get()};
  winrt::com_ptr<ID2D1Factory1> spD2dFactory;
  compContext.as<::Microsoft::ReactNative::Composition::Experimental::ICompositionContextInterop>()->D2DFactory(
      spD2dFactory.put());

  winrt::com_ptr<ID2D1GeometryGroup> geometryGroup = nullptr;
  // Create a geometry group.
  HRESULT hr = spD2dFactory->CreateGeometryGroup(
      D2D1_FILL_MODE_ALTERNATE, ppGeometries, ARRAYSIZE(ppGeometries), geometryGroup.put());

  if (SUCCEEDED(hr)) {
    return geometryGroup;
  }
  return nullptr;
}

BorderPrimitive::BorderPrimitive(
    winrt::Microsoft::ReactNative::Composition::implementation::ComponentView &outer,
    const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &rootVisual)
    : m_outer(&outer), m_rootVisual(rootVisual) {}

BorderPrimitive::BorderPrimitive(winrt::Microsoft::ReactNative::Composition::implementation::ComponentView &outer)
    : m_outer(&outer), m_rootVisual(outer.CompositionContext().CreateSpriteVisual()) {}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual BorderPrimitive::RootVisual() const noexcept {
  return m_rootVisual;
}

bool BorderPrimitive::requiresBorder(
    const facebook::react::BorderMetrics &borderMetrics,
    winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme) noexcept {
  // We only handle a single borderStyle for now
  auto borderStyle = borderMetrics.borderStyles.left;

  bool hasMeaningfulColor =
      !borderMetrics.borderColors.isUniform() || !isColorMeaningful(borderMetrics.borderColors.left, theme);
  bool hasMeaningfulWidth = !borderMetrics.borderWidths.isUniform() || (borderMetrics.borderWidths.left != 0);
  if (!hasMeaningfulColor && !hasMeaningfulWidth) {
    return false;
  }
  return true;
}

void BorderPrimitive::updateProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  if (oldViewProps.borderColors != newViewProps.borderColors || oldViewProps.borderRadii != newViewProps.borderRadii ||
      !(oldViewProps.yogaStyle.border(facebook::yoga::Edge::All) ==
        newViewProps.yogaStyle.border(facebook::yoga::Edge::All)) ||
      oldViewProps.borderStyles != newViewProps.borderStyles) {
    m_needsUpdate = true;
  }
}

void BorderPrimitive::markNeedsUpdate() noexcept {
  m_needsUpdate = true;
}

void BorderPrimitive::finalize(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::BorderMetrics &borderMetrics) noexcept {
  if (!m_needsUpdate) {
    return;
  }

  auto theme = m_outer->theme();
  if (!theme || theme->IsEmpty()) {
    return;
  }

  m_needsUpdate = false;
  auto spBorderLayers = FindSpecialBorderLayers();

  if (!TryUpdateSpecialBorderLayers(m_outer->theme(), spBorderLayers, layoutMetrics, borderMetrics)) {
    for (auto &spBorderLayer : spBorderLayers) {
      if (spBorderLayer) {
        spBorderLayer.as<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual>().Brush(nullptr);
      }
    }
  }
}

void BorderPrimitive::onThemeChanged(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::BorderMetrics &borderMetrics) noexcept {
  m_needsUpdate = true;
  finalize(layoutMetrics, borderMetrics);
}

std::array<
    winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual,
    BorderPrimitive::SpecialBorderLayerCount>
BorderPrimitive::FindSpecialBorderLayers() const noexcept {
  std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount> layers{
      nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

  if (m_numBorderVisuals) {
    for (uint8_t i = 0; i < m_numBorderVisuals; i++) {
      auto visual = m_rootVisual.GetAt(i);
      layers[i] = visual.as<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual>();
    }
  }

  return layers;
}

uint8_t BorderPrimitive::numberOfVisuals() const noexcept {
  return m_numBorderVisuals;
}

void BorderPrimitive::setOuter(
    winrt::Microsoft::ReactNative::Composition::implementation::ComponentView *outer) noexcept {
  m_outer = outer;
}

bool BorderPrimitive::TryUpdateSpecialBorderLayers(
    winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme,
    std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount>
        &spBorderVisuals,
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::BorderMetrics &borderMetrics) noexcept {
  // We only handle a single borderStyle for now
  auto borderStyle = borderMetrics.borderStyles.left;

  bool hasMeaningfulColor =
      !borderMetrics.borderColors.isUniform() || !isColorMeaningful(borderMetrics.borderColors.left, theme);
  bool hasMeaningfulWidth = !borderMetrics.borderWidths.isUniform() || (borderMetrics.borderWidths.left != 0);
  if (!hasMeaningfulColor && !hasMeaningfulWidth) {
    return false;
  }

  // Create the special border layers if they don't exist yet
  if (!spBorderVisuals[0]) {
    for (uint8_t i = 0; i < SpecialBorderLayerCount; i++) {
      auto visual = m_outer->CompositionContext().CreateSpriteVisual();
      m_rootVisual.InsertAt(visual, i);
      spBorderVisuals[i] = std::move(visual);
      m_numBorderVisuals++;
    }
  }

  float extentWidth = layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor;
  float extentHeight = layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor;

  if (borderMetrics.borderRadii.topLeft != 0 || borderMetrics.borderRadii.bottomLeft != 0 ||
      borderMetrics.borderRadii.topRight != 0 || borderMetrics.borderRadii.bottomRight != 0) {
    auto compContext = m_outer->CompositionContext();
    if (borderStyle == facebook::react::BorderStyle::Dotted || borderStyle == facebook::react::BorderStyle::Dashed) {
      // Because in DirectX geometry starts at the center of the stroke, we need to deflate
      // rectangle by half the stroke width to render correctly.
      facebook::react::RectangleEdges<float> rectPathGeometry = {
          borderMetrics.borderWidths.left / 2.0f,
          borderMetrics.borderWidths.top / 2.0f,
          extentWidth - borderMetrics.borderWidths.right / 2.0f,
          extentHeight - borderMetrics.borderWidths.bottom / 2.0f};

      winrt::com_ptr<ID2D1PathGeometry> pathGeometry =
          GenerateRoundedRectPathGeometry(compContext, borderMetrics.borderRadii, {0, 0, 0, 0}, rectPathGeometry);

      if (pathGeometry) {
        DrawAllBorderLayers(
            theme,
            compContext,
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
          compContext,
          borderMetrics.borderRadii,
          {0, 0, 0, 0}, // inset
          borderMetrics.borderWidths,
          rectPathGeometry);

      DrawAllBorderLayers(
          theme,
          compContext,
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
    auto compContext = m_outer->CompositionContext();
    // Because in DirectX geometry starts at the center of the stroke, we need to deflate rectangle by half the stroke
    // width / height to render correctly.
    D2D1_RECT_F rectShape{
        borderMetrics.borderWidths.left / 2.0f,
        borderMetrics.borderWidths.top / 2.0f,
        extentWidth - (borderMetrics.borderWidths.right / 2.0f),
        extentHeight - (borderMetrics.borderWidths.bottom / 2.0f)};
    DrawAllBorderLayers(
        theme,
        compContext,
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

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
