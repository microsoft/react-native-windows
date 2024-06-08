
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompositionViewComponentView.h"

#include <AutoDraw.h>
#include <Fabric/AbiState.h>
#include <Fabric/AbiViewProps.h>
#include <Fabric/Composition/ReactNativeIsland.h>
#include <Fabric/FabricUIManagerModule.h>
#include <UI.Xaml.Controls.h>
#include <Utils/KeyboardUtils.h>
#include <Utils/ValueUtils.h>
#include <Views/FrameworkElementTransferProperties.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionContextHelper.h"
#include "CompositionDynamicAutomationProvider.h"
#include "CompositionHelpers.h"
#include "RootComponentView.h"
#include "Theme.h"
#include "UiaHelpers.h"
#include "d2d1helper.h"

#include "Composition.ComponentView.g.cpp"
#include "Composition.ViewComponentView.g.cpp"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

CreateCompositionComponentViewArgs::CreateCompositionComponentViewArgs(
    const winrt::Microsoft::ReactNative::IReactContext &reactContext,
    facebook::react::Tag tag,
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compositionContext)
    : base_type(reactContext, tag), m_compositionContext(compositionContext){};

winrt::Microsoft::UI::Composition::Compositor CreateCompositionComponentViewArgs::Compositor() const noexcept {
  return winrt::Microsoft::ReactNative::Composition::CompositionUIService::GetCompositor(ReactContext().Properties());
}

winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext
CreateCompositionComponentViewArgs::CompositionContext() const noexcept {
  return m_compositionContext;
}

ComponentViewFeatures CreateCompositionComponentViewArgs::Features() const noexcept {
  return m_features;
}

void CreateCompositionComponentViewArgs::Features(ComponentViewFeatures value) noexcept {
  m_features = value;
}

ComponentView::ComponentView(const winrt::Microsoft::ReactNative::Composition::CreateCompositionComponentViewArgs &args)
    : ComponentView(
          winrt::get_self<
              winrt::Microsoft::ReactNative::Composition::implementation::CreateCompositionComponentViewArgs>(args)
              ->CompositionContext(),
          args.Tag(),
          args.ReactContext(),
          args.Features(),
          true) {}

ComponentView::ComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    ComponentViewFeatures flags,
    bool customControl)
    : base_type(tag, reactContext, customControl), m_compContext(compContext), m_flags(flags) {
  m_outerVisual = compContext.CreateSpriteVisual(); // TODO could be a raw ContainerVisual if we had a
                                                    // CreateContainerVisual in ICompositionContext
  m_focusVisual = compContext.CreateFocusVisual();
  m_outerVisual.InsertAt(m_focusVisual.InnerVisual(), 0);
}

facebook::react::Tag ComponentView::Tag() const noexcept {
  return m_tag;
}

facebook::react::Props::Shared ComponentView::props() noexcept {
  return viewProps();
}

void ComponentView::onThemeChanged() noexcept {
  if ((m_flags & ComponentViewFeatures::Background) == ComponentViewFeatures::Background) {
    if (viewProps()->backgroundColor) {
      Visual().as<Experimental::ISpriteVisual>().Brush(theme()->Brush(*viewProps()->backgroundColor));
    } else {
      Visual().as<Experimental::ISpriteVisual>().Brush(nullptr);
    }
  }

  if ((m_flags & ComponentViewFeatures::NativeBorder) == ComponentViewFeatures::NativeBorder) {
    m_needsBorderUpdate = true;
    finalizeBorderUpdates(m_layoutMetrics, *viewProps());
  }

  if ((m_flags & ComponentViewFeatures::ShadowProps) == ComponentViewFeatures::ShadowProps) {
    applyShadowProps(*viewProps());
  }

  base_type::onThemeChanged();

  if (m_customComponent) {
    // Review is it expected that I need this cast to call overridden methods?
    winrt::Microsoft::ReactNative::Composition::ComponentView outer(*this);
    outer.OnThemeChanged();
  }
}

void ComponentView::OnThemeChanged() noexcept {}

void ComponentView::Theme(const winrt::Microsoft::ReactNative::Composition::Theme &value) noexcept {
  theme(winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(value));
}

winrt::Microsoft::ReactNative::Composition::Theme ComponentView::Theme() const noexcept {
  return theme()->get_strong().as<winrt::Microsoft::ReactNative::Composition::Theme>();
}

winrt::Microsoft::ReactNative::Composition::RootComponentView ComponentView::Root() noexcept {
  return *rootComponentView();
}

winrt::Microsoft::UI::Composition::Compositor ComponentView::Compositor() const noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(
      m_compContext);
}

winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext ComponentView::CompositionContext()
    const noexcept {
  return m_compContext;
}

void ComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps =
      oldProps ? (*std::static_pointer_cast<const facebook::react::ViewProps>(oldProps)) : (*viewProps());
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  if ((m_flags & ComponentViewFeatures::Background) == ComponentViewFeatures::Background) {
    if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
      if (newViewProps.backgroundColor) {
        Visual().as<Experimental::ISpriteVisual>().Brush(theme()->Brush(*newViewProps.backgroundColor));
      } else {
        Visual().as<Experimental::ISpriteVisual>().Brush(nullptr);
      }
    }
  }

  if ((m_flags & ComponentViewFeatures::NativeBorder) == ComponentViewFeatures::NativeBorder) {
    updateBorderProps(oldViewProps, newViewProps);
  }
  if ((m_flags & ComponentViewFeatures::ShadowProps) == ComponentViewFeatures::ShadowProps) {
    updateShadowProps(oldViewProps, newViewProps);
  }

  base_type::updateProps(props, oldProps);
}

void ComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  if ((m_flags & ComponentViewFeatures::NativeBorder) == ComponentViewFeatures::NativeBorder) {
    updateBorderLayoutMetrics(layoutMetrics, *viewProps());
  }

  m_layoutMetrics = layoutMetrics;
  UpdateCenterPropertySet();

  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

const facebook::react::LayoutMetrics &ComponentView::layoutMetrics() const noexcept {
  return m_layoutMetrics;
}

void ComponentView::FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  if ((m_flags & ComponentViewFeatures::NativeBorder) == ComponentViewFeatures::NativeBorder) {
    finalizeBorderUpdates(m_layoutMetrics, *viewProps());
  }

  base_type::FinalizeUpdates(updateMask);
}

void ComponentView::onLostFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  if (args.OriginalSource() == Tag()) {
    m_eventEmitter->onBlur();
    showFocusVisual(false);
    if (m_uiaProvider) {
      winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
          m_uiaProvider, UIA_HasKeyboardFocusPropertyId, true, false);
    }
  }
  base_type::onLostFocus(args);
}

void ComponentView::onGotFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  if (args.OriginalSource() == Tag()) {
    m_eventEmitter->onFocus();
    if (m_enableFocusVisual) {
      showFocusVisual(true);
    }
    if (m_uiaProvider) {
      auto spProviderSimple = m_uiaProvider.try_as<IRawElementProviderSimple>();
      if (spProviderSimple != nullptr) {
        winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
            m_uiaProvider, UIA_HasKeyboardFocusPropertyId, false, true);
        UiaRaiseAutomationEvent(spProviderSimple.get(), UIA_AutomationFocusChangedEventId);
      }
    }

    StartBringIntoView({});
  }
  base_type::onGotFocus(args);
}

void ComponentView::StartBringIntoView(
    winrt::Microsoft::ReactNative::implementation::BringIntoViewOptions &&options) noexcept {
  if (!options.TargetRect) {
    // Default to bring the entire of this component into view
    options.TargetRect = {
        {0, 0},
        {m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
         m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor}};
  }

  if (m_parent) {
    options.TargetRect->origin.y += m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor;
    options.TargetRect->origin.x += m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor;
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->StartBringIntoView(
        std::move(options));
  }
}

void ComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  m_eventEmitter = std::static_pointer_cast<facebook::react::ViewEventEmitter const>(eventEmitter);
}

void ComponentView::HandleCommand(
    winrt::hstring commandName,
    const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept {
  if (commandName == L"focus") {
    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*get_strong());
    }
    return;
  }
  if (commandName == L"blur") {
    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(nullptr); // Todo store this component as previously focused element
    }
    return;
  }
  assert(false); // Unhandled command
}

bool ComponentView::CapturePointer(const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer) noexcept {
  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return false;

  auto root = rootComponentView();
  if (!root)
    return false;

  auto rootView{uiManager->GetReactNativeIsland(root->Tag())};
  if (!rootView) {
    return false;
  }

  return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->CapturePointer(
      pointer, static_cast<facebook::react::Tag>(Tag()));
}

void ComponentView::ReleasePointerCapture(
    const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer) noexcept {
  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return;

  auto root = rootComponentView();
  if (!root)
    return;

  auto rootView{uiManager->GetReactNativeIsland(root->Tag())};
  if (!rootView) {
    return;
  }

  return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)
      ->ReleasePointerCapture(pointer, static_cast<facebook::react::Tag>(Tag()));
}

RECT ComponentView::getClientRect() const noexcept {
  RECT rc{0};
  facebook::react::Point parentOffset{0};
  if (m_parent) {
    parentOffset =
        winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->getClientOffset();
  }

  rc.left = static_cast<LONG>((m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor) + parentOffset.x);
  rc.top += static_cast<LONG>((m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor) + parentOffset.y);
  rc.right = rc.left + static_cast<LONG>(m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor);
  rc.bottom = rc.top + static_cast<LONG>(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor);
  return rc;
}

facebook::react::Point ComponentView::getClientOffset() const noexcept {
  facebook::react::Point parentOffset{0};
  if (m_parent) {
    parentOffset =
        winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->getClientOffset();
  }

  return {
      (m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor) + parentOffset.x,
      (m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor) + parentOffset.y};
}

const facebook::react::SharedViewEventEmitter &ComponentView::GetEventEmitter() const noexcept {
  return m_eventEmitter;
}

std::array<
    winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual,
    ComponentView::SpecialBorderLayerCount>
ComponentView::FindSpecialBorderLayers() const noexcept {
  std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount> layers{
      nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

  if (m_numBorderVisuals) {
    for (uint8_t i = 0; i < m_numBorderVisuals; i++) {
      auto visual = Visual().GetAt(i);
      layers[i] = visual.as<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual>();
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

static winrt::com_ptr<ID2D1PathGeometry> GenerateRoundedRectPathGeometry(
    winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
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

    if (!facebook::react::isColorMeaningful(borderColor)) {
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
      layer.Size({textureRect.right - textureRect.left, textureRect.bottom - textureRect.top});

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
        ComponentView::SpecialBorderLayerCount> &spBorderLayers,
    TShape &shape,
    const facebook::react::BorderWidths &borderWidths,
    const facebook::react::BorderRadii &borderRadii,
    float textureWidth,
    float textureHeight,
    const facebook::react::BorderColors &borderColors,
    facebook::react::BorderStyle borderStyle) {
  // Now that we've drawn our nice border in one layer, split it into its component layers
  winrt::com_ptr<::Microsoft::ReactNative::Composition::Experimental::ICompositionDrawingSurfaceInterop>
      spTextures[ComponentView::SpecialBorderLayerCount];

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

facebook::react::BorderMetrics resolveAndAlignBorderMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = viewProps.resolveBorderMetrics(layoutMetrics);

  pixelRoundBorderRadii(borderMetrics.borderRadii, layoutMetrics.pointScaleFactor);
  scaleAndPixelRoundBorderWidths(layoutMetrics, borderMetrics, layoutMetrics.pointScaleFactor);
  assignDefaultBlackBorders(borderMetrics);
  return borderMetrics;
}

bool ComponentView::TryUpdateSpecialBorderLayers(
    winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme,
    std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount>
        &spBorderVisuals,
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = resolveAndAlignBorderMetrics(layoutMetrics, viewProps);
  // We only handle a single borderStyle for now
  auto borderStyle = borderMetrics.borderStyles.left;

  bool hasMeaningfulColor =
      !borderMetrics.borderColors.isUniform() || !facebook::react::isColorMeaningful(borderMetrics.borderColors.left);
  bool hasMeaningfulWidth = !borderMetrics.borderWidths.isUniform() || (borderMetrics.borderWidths.left != 0);
  if (!hasMeaningfulColor && !hasMeaningfulWidth) {
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
            theme,
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
          theme,
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
        theme,
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

void ComponentView::finalizeBorderUpdates(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  if (!m_needsBorderUpdate || theme()->IsEmpty()) {
    return;
  }

  m_needsBorderUpdate = false;
  auto spBorderLayers = FindSpecialBorderLayers();

  if (!TryUpdateSpecialBorderLayers(theme(), spBorderLayers, layoutMetrics, viewProps)) {
    for (auto &spBorderLayer : spBorderLayers) {
      if (spBorderLayer) {
        spBorderLayer.as<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual>().Brush(nullptr);
      }
    }
  }
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ComponentView::OuterVisual() const noexcept {
  return m_outerVisual ? m_outerVisual : Visual();
}

void ComponentView::showFocusVisual(bool show) noexcept {
  if (show) {
    assert(m_enableFocusVisual);
    m_focusVisual.IsFocused(true);
  } else {
    m_focusVisual.IsFocused(false);
  }
}

void ComponentView::updateBorderProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  if (oldViewProps.borderColors != newViewProps.borderColors || oldViewProps.borderRadii != newViewProps.borderRadii ||
      !(oldViewProps.yogaStyle.border(facebook::yoga::Edge::All) ==
        newViewProps.yogaStyle.border(facebook::yoga::Edge::All)) ||
      oldViewProps.borderStyles != newViewProps.borderStyles) {
    m_needsBorderUpdate = true;
  }

  m_enableFocusVisual = newViewProps.enableFocusRing;
  if (!m_enableFocusVisual) {
    showFocusVisual(false);
  }
}

void ComponentView::updateShadowProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  // Shadow Properties
  if (oldViewProps.shadowOffset != newViewProps.shadowOffset || oldViewProps.shadowColor != newViewProps.shadowColor ||
      oldViewProps.shadowOpacity != newViewProps.shadowOpacity ||
      oldViewProps.shadowRadius != newViewProps.shadowRadius) {
    applyShadowProps(newViewProps);
  }
}

void ComponentView::applyShadowProps(const facebook::react::ViewProps &viewProps) noexcept {
  auto shadow = m_compContext.CreateDropShadow();
  shadow.Offset({viewProps.shadowOffset.width, viewProps.shadowOffset.height, 0});
  shadow.Opacity(viewProps.shadowOpacity);
  shadow.BlurRadius(viewProps.shadowRadius);
  if (viewProps.shadowColor)
    shadow.Color(theme()->Color(*viewProps.shadowColor));
  Visual().as<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual>().Shadow(shadow);
}

void ComponentView::updateTransformProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps,
    winrt::Microsoft::ReactNative::Composition::Experimental::IVisual visual) noexcept {
  // check for backfaceVisibility prop
  if (oldViewProps.backfaceVisibility != newViewProps.backfaceVisibility) {
    static_assert(
        static_cast<facebook::react::BackfaceVisibility>(
            winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility::Inherit) ==
        facebook::react::BackfaceVisibility::Auto);
    static_assert(
        static_cast<facebook::react::BackfaceVisibility>(
            winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility::Visible) ==
        facebook::react::BackfaceVisibility::Visible);
    static_assert(
        static_cast<facebook::react::BackfaceVisibility>(
            winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility::Hidden) ==
        facebook::react::BackfaceVisibility::Hidden);
    visual.BackfaceVisibility(static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility>(
        newViewProps.backfaceVisibility));
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
    if (centerPointPropSet) {
      centerPointPropSet.InsertMatrix4x4(L"transform", transformMatrix);
    }

    EnsureTransformMatrixFacade();
  }
}

void ComponentView::updateAccessibilityProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  if (!m_uiaProvider)
    return;

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      m_uiaProvider, UIA_IsKeyboardFocusablePropertyId, oldViewProps.focusable, newViewProps.focusable);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      m_uiaProvider,
      UIA_NamePropertyId,
      oldViewProps.accessibilityLabel,
      newViewProps.accessibilityLabel.empty() ? DefaultAccessibleName() : newViewProps.accessibilityLabel);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      m_uiaProvider,
      UIA_IsContentElementPropertyId,
      (oldViewProps.accessible && oldViewProps.accessibilityRole != "none"),
      (newViewProps.accessible && newViewProps.accessibilityRole != "none"));

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      m_uiaProvider,
      UIA_IsControlElementPropertyId,
      (oldViewProps.accessible && oldViewProps.accessibilityRole != "none"),
      (newViewProps.accessible && newViewProps.accessibilityRole != "none"));

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      m_uiaProvider,
      UIA_IsEnabledPropertyId,
      !(oldViewProps.accessibilityState && oldViewProps.accessibilityState->disabled),
      !(newViewProps.accessibilityState && newViewProps.accessibilityState->disabled));

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      m_uiaProvider, UIA_ControlTypePropertyId, oldViewProps.accessibilityRole, newViewProps.accessibilityRole);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      m_uiaProvider, UIA_HelpTextPropertyId, oldViewProps.accessibilityHint, newViewProps.accessibilityHint);
}

std::optional<std::string> ComponentView::getAcccessiblityValue() noexcept {
  return std::static_pointer_cast<const facebook::react::ViewProps>(props())->accessibilityValue.text;
}

void ComponentView::setAcccessiblityValue(std::string &&value) noexcept {
  // no-op
}

bool ComponentView::getAcccessiblityIsReadOnly() noexcept {
  return true;
}

void ComponentView::updateBorderLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = resolveAndAlignBorderMetrics(layoutMetrics, viewProps);

  if (borderMetrics.borderRadii.topLeft == 0 && borderMetrics.borderRadii.topRight == 0 &&
      borderMetrics.borderRadii.bottomLeft == 0 && borderMetrics.borderRadii.bottomRight == 0) {
    Visual().as<::Microsoft::ReactNative::Composition::Experimental::IVisualInterop>()->SetClippingPath(nullptr);
  } else {
    winrt::com_ptr<ID2D1PathGeometry> pathGeometry = GenerateRoundedRectPathGeometry(
        m_compContext,
        borderMetrics.borderRadii,
        {0, 0, 0, 0},
        {0,
         0,
         layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
         layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});

    Visual().as<::Microsoft::ReactNative::Composition::Experimental::IVisualInterop>()->SetClippingPath(
        pathGeometry.get());
  }

  if (m_layoutMetrics != layoutMetrics) {
    m_needsBorderUpdate = true;
  }

  m_focusVisual.ScaleFactor(layoutMetrics.pointScaleFactor);
  OuterVisual().Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  OuterVisual().Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}

void ComponentView::indexOffsetForBorder(uint32_t &index) const noexcept {
  index += m_numBorderVisuals;
}

void ComponentView::OnRenderingDeviceLost() noexcept {}

std::vector<facebook::react::ComponentDescriptorProvider>
ComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

comp::CompositionPropertySet ComponentView::EnsureCenterPointPropertySet() noexcept {
  if (m_centerPropSet == nullptr) {
    if (auto compositor =
            winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerCompositor(
                m_compContext)) {
      m_centerPropSet = compositor.CreatePropertySet();
      UpdateCenterPropertySet();
      m_centerPropSet.InsertMatrix4x4(L"transform", winrt::Windows::Foundation::Numerics::float4x4::identity());
      m_centerPropSet.InsertVector3(L"translation", {0, 0, 0});
    }
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
void ComponentView::UpdateCenterPropertySet() noexcept {
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

void ComponentView::EnsureTransformMatrixFacade() noexcept {
  if (m_hasTransformMatrixFacade)
    return;
  m_hasTransformMatrixFacade = true;

  auto centerPointPropSet = EnsureCenterPointPropertySet();
  if (auto compositor =
          winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerCompositor(
              m_compContext)) {
    // TODO cache expression instead of creating new ones all the time
    auto expression = compositor.CreateExpressionAnimation(
        L"Matrix4x4.CreateFromScale(PS.dpiScale3Inv) * Matrix4x4.CreateFromTranslation(PS.translation) * PS.transform * Matrix4x4.CreateFromScale(PS.dpiScale3)");
    expression.SetReferenceParameter(L"PS", centerPointPropSet);
    winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(OuterVisual())
        .StartAnimation(L"TransformMatrix", expression);
  }
}

facebook::react::SharedViewEventEmitter ComponentView::eventEmitter() noexcept {
  return m_eventEmitter;
}

facebook::react::SharedViewEventEmitter ComponentView::eventEmitterAtPoint(facebook::react::Point /*pt*/) noexcept {
  return m_eventEmitter;
}

bool ComponentView::anyHitTestHelper(
    facebook::react::Tag &targetTag,
    facebook::react::Point &ptContent,
    facebook::react::Point &localPt) const noexcept {
  if (auto index = m_children.Size()) {
    do {
      index--;
      targetTag = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_children.GetAt(index))
                      ->hitTest(ptContent, localPt);
      if (targetTag != -1) {
        return true;
      }
    } while (index != 0);
  }

  return false;
}

std::string ComponentView::DefaultControlType() const noexcept {
  return "group";
}

std::string ComponentView::DefaultAccessibleName() const noexcept {
  return "";
}

std::string ComponentView::DefaultHelpText() const noexcept {
  return "";
}

ViewComponentView::ViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::CreateCompositionComponentViewArgs &args)
    : ViewComponentView(
          ViewComponentView::defaultProps(),
          winrt::get_self<
              winrt::Microsoft::ReactNative::Composition::implementation::CreateCompositionComponentViewArgs>(args)
              ->CompositionContext(),
          args.Tag(),
          args.ReactContext(),
          args.Features(),
          true) {}

facebook::react::SharedViewProps ViewComponentView::defaultProps() noexcept {
  static auto const defaultViewProps = std::make_shared<facebook::react::ViewProps const>();
  return defaultViewProps;
}

ViewComponentView::ViewComponentView(
    const facebook::react::SharedViewProps &defaultProps,
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    ComponentViewFeatures flags,
    bool customComponent)
    : base_type(compContext, tag, reactContext, flags, customComponent),
      m_props(defaultProps ? defaultProps : ViewComponentView::defaultProps()) {}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ViewComponentView::createVisual() noexcept {
  return m_compContext.CreateSpriteVisual();
}

winrt::Microsoft::UI::Composition::Visual ViewComponentView::CreateVisual() noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerVisual(
      createVisual());
}

void ViewComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    if (m_customComponent) {
      // Review is it expected that I need this cast to call overridden methods?
      winrt::Microsoft::ReactNative::Composition::ViewComponentView outer(*this);
      winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCreateVisual internalCreateVisual{nullptr};
      if (outer.try_as(internalCreateVisual)) {
        m_visual = internalCreateVisual.CreateInternalVisual();
      } else {
        m_visual =
            winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::CreateVisual(
                outer.CreateVisual());
      }
    } else {
      m_visual = createVisual();
    }
    OuterVisual().InsertAt(m_visual, 0);
  }
}

winrt::Microsoft::ReactNative::ComponentView ViewComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ViewComponentView>(
      ViewComponentView::defaultProps(), compContext, tag, reactContext, ComponentViewFeatures::Default, false);
}

void ViewComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::MountChildComponentView(childComponentView, index);

  indexOffsetForBorder(index);
  ensureVisual();

  // TODO if we get mixed children of composition and non-composition ComponentViews the indexes will get mixed up
  // We could offset the index based on non-composition children in m_children
  if (auto compositionChild = childComponentView.try_as<ComponentView>()) {
    Visual().InsertAt(compositionChild->OuterVisual(), index);
  }
}

void ViewComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::UnmountChildComponentView(childComponentView, index);

  indexOffsetForBorder(index);
  if (auto compositionChild = childComponentView.try_as<ComponentView>()) {
    Visual().Remove(compositionChild->OuterVisual());
  }
}

void ViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(oldProps ? oldProps : m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  ensureVisual();
  if (oldViewProps.opacity != newViewProps.opacity) {
    Visual().Opacity(newViewProps.opacity);
  }
  if (oldViewProps.testId != newViewProps.testId) {
    Visual().Comment(winrt::to_hstring(newViewProps.testId));
  }

  // update BaseComponentView props
  updateAccessibilityProps(oldViewProps, newViewProps);
  updateTransformProps(oldViewProps, newViewProps, Visual());
  base_type::updateProps(props, oldProps);

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

const winrt::Microsoft::ReactNative::IComponentProps ViewComponentView::userProps(
    facebook::react::Props::Shared const &props) noexcept {
  assert(m_customComponent);
  const auto &abiViewProps = *std::static_pointer_cast<const ::Microsoft::ReactNative::AbiViewProps>(props);
  return abiViewProps.UserProps();
}

facebook::react::Tag ViewComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag targetTag = -1;

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
      anyHitTestHelper(targetTag, ptLocal, localPt))
    return targetTag;

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }

  return -1;
}

inline winrt::Windows::System::VirtualKey GetLeftOrRightModifiedKey(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    winrt::Windows::System::VirtualKey leftKey,
    winrt::Windows::System::VirtualKey rightKey) {
  return (source.GetKeyState(leftKey) == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down) ? leftKey : rightKey;
}

std::string CodeFromVirtualKey(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    winrt::Windows::System::VirtualKey virtualKey) {
  int key = static_cast<int>(virtualKey);

  if (isdigit(key)) {
    return "Digit" + std::string(1, static_cast<char>(key));
  } else if (isupper(key)) {
    return "Key" + std::string(1, static_cast<char>(key));
  } else {
    // Override the virtual key if it's modified key of Control, Shift or Menu
    if (virtualKey == winrt::Windows::System::VirtualKey::Control) {
      virtualKey = GetLeftOrRightModifiedKey(
          source, winrt::Windows::System::VirtualKey::LeftControl, winrt::Windows::System::VirtualKey::RightControl);
    } else if (virtualKey == winrt::Windows::System::VirtualKey::Shift) {
      virtualKey = GetLeftOrRightModifiedKey(
          source, winrt::Windows::System::VirtualKey::LeftShift, winrt::Windows::System::VirtualKey::RightShift);
    } else if (virtualKey == winrt::Windows::System::VirtualKey::Menu) {
      virtualKey = GetLeftOrRightModifiedKey(
          source, winrt::Windows::System::VirtualKey::LeftMenu, winrt::Windows::System::VirtualKey::RightMenu);
    }
  }

  return ::Microsoft::ReactNative::GetOrUnidentifiedCode(virtualKey);
}

void ViewComponentView::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  auto eventCode = CodeFromVirtualKey(source, args.Key());
  bool fShift = source.GetKeyState(winrt::Windows::System::VirtualKey::Shift) !=
      winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
  bool fAlt = source.GetKeyState(winrt::Windows::System::VirtualKey::Menu) !=
      winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
  bool fCtrl = source.GetKeyState(winrt::Windows::System::VirtualKey::Control) !=
      winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
  bool fMeta = (source.GetKeyState(winrt::Windows::System::VirtualKey::LeftWindows) !=
                winrt::Windows::UI::Core::CoreVirtualKeyStates::None) ||
      (source.GetKeyState(winrt::Windows::System::VirtualKey::RightWindows) !=
       winrt::Windows::UI::Core::CoreVirtualKeyStates::None);

  if (args.OriginalSource() == Tag() && !args.Handled()) {
    facebook::react::KeyEvent event;
    event.shiftKey = fShift;
    event.ctrlKey = fCtrl;
    event.altKey = fAlt;
    event.metaKey = fMeta;

    event.key = ::Microsoft::ReactNative::FromVirtualKey(args.Key(), event.shiftKey, !!(GetKeyState(VK_CAPITAL) & 1));
    event.code = eventCode;
    m_eventEmitter->onKeyDown(event);
  }

  for (const auto &handledKey : m_props->keyDownEvents) {
    if (handledKey.code == eventCode && handledKey.altKey == fAlt && handledKey.ctrlKey == fCtrl &&
        handledKey.metaKey == fMeta && handledKey.shiftKey == fShift &&
        handledKey.handledEventPhase == facebook::react::HandledEventPhase::Bubbling) {
      // Stop bubbling event if marked as handled from JS
      args.Handled(true);
    }
  }

  base_type::OnKeyDown(source, args);
}

void ViewComponentView::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  auto eventCode = CodeFromVirtualKey(source, args.Key());
  bool fShift = source.GetKeyState(winrt::Windows::System::VirtualKey::Shift) !=
      winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
  bool fAlt = source.GetKeyState(winrt::Windows::System::VirtualKey::Menu) !=
      winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
  bool fCtrl = source.GetKeyState(winrt::Windows::System::VirtualKey::Control) !=
      winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
  bool fMeta = (source.GetKeyState(winrt::Windows::System::VirtualKey::LeftWindows) !=
                winrt::Windows::UI::Core::CoreVirtualKeyStates::None) ||
      (source.GetKeyState(winrt::Windows::System::VirtualKey::RightWindows) !=
       winrt::Windows::UI::Core::CoreVirtualKeyStates::None);

  if (args.OriginalSource() == Tag()) {
    facebook::react::KeyEvent event;
    event.shiftKey = fShift;
    event.ctrlKey = fCtrl;
    event.altKey = fAlt;
    event.metaKey = fMeta;

    event.key = ::Microsoft::ReactNative::FromVirtualKey(args.Key(), event.shiftKey, !!(GetKeyState(VK_CAPITAL) & 1));
    event.code = eventCode;
    m_eventEmitter->onKeyUp(event);
  }

  for (const auto &handledKey : m_props->keyUpEvents) {
    if (handledKey.code == eventCode && handledKey.altKey == fAlt && handledKey.ctrlKey == fCtrl &&
        handledKey.metaKey == fMeta && handledKey.shiftKey == fShift &&
        handledKey.handledEventPhase == facebook::react::HandledEventPhase::Bubbling) {
      // Stop bubbling event if marked as handled from JS
      args.Handled(true);
    }
  }

  base_type::OnKeyUp(source, args);
}

void ViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }
  ensureVisual();
  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

void ViewComponentView::UpdateLayoutMetrics(const LayoutMetrics &metrics, const LayoutMetrics &oldMetrics) noexcept {
  Visual().Size({metrics.Frame.Width * metrics.PointScaleFactor, metrics.Frame.Height * metrics.PointScaleFactor});
}

void ViewComponentView::prepareForRecycle() noexcept {}

const facebook::react::SharedViewProps &ViewComponentView::viewProps() const noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::ViewProps ViewComponentView::ViewProps() noexcept {
  return ViewPropsInner();
}

winrt::Microsoft::ReactNative::ViewProps ViewComponentView::ViewPropsInner() noexcept {
  // If we have AbiViewProps, then we dont need to new up a props wrapper
  if (m_customComponent) {
    const auto &abiViewProps = *std::static_pointer_cast<const ::Microsoft::ReactNative::AbiViewProps>(m_props);
    return abiViewProps.ViewProps();
  }

  return winrt::make<winrt::Microsoft::ReactNative::implementation::ViewProps>(m_props);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ViewComponentView::Visual() const noexcept {
  assert(m_visual);
  return m_visual;
}

bool ViewComponentView::focusable() const noexcept {
  return m_props->focusable;
}

std::string ViewComponentView::DefaultControlType() const noexcept {
  return "group";
}

winrt::IInspectable ComponentView::EnsureUiaProvider() noexcept {
  if (m_uiaProvider == nullptr) {
    m_uiaProvider =
        winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>(*get_strong());
  }
  return m_uiaProvider;
}

bool IntersectRect(RECT *prcDst, const RECT &prcSrc1, const RECT &prcSrc2) {
  prcDst->left = std::max(prcSrc1.left, prcSrc2.left);
  prcDst->right = std::min(prcSrc1.right, prcSrc2.right);

  if (prcDst->left < prcDst->right) {
    prcDst->top = std::max(prcSrc1.top, prcSrc2.top);
    prcDst->bottom = std::min(prcSrc1.bottom, prcSrc2.bottom);

    if (prcDst->top < prcDst->bottom) {
      return true;
    }
  }

  prcDst->left = prcDst->top = prcDst->right = prcDst->bottom = 0;
  return false;
}

winrt::Microsoft::ReactNative::implementation::ClipState ComponentView::getClipState() noexcept {
  if (!m_parent) {
    return winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped;
  }

  RECT intersection;
  const auto parentRect =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->getClientRect();
  const auto clientRect = getClientRect();

  IntersectRect(&intersection, parentRect, clientRect);

  if (intersection == clientRect) {
    return m_parent.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()->getClipState();
  }

  if (((intersection.right - intersection.left) == 0) && ((intersection.bottom - intersection.top) == 0)) {
    return winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped;
  }

  if (m_parent.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()->getClipState() ==
      winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped) {
    return winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped;
  }

  return winrt::Microsoft::ReactNative::implementation::ClipState::PartialClip;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::implementation {

winrt::Microsoft::ReactNative::ComponentView lastDeepChild(
    const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  auto current = view;
  while (current) {
    auto children = current.Children();
    if (children.Size() == 0)
      break;
    current = children.GetAt(children.Size() - 1);
  }
  return current;
}

bool walkTree(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    bool forward,
    Mso::Functor<bool(const winrt::Microsoft::ReactNative::ComponentView &)> &fn) noexcept {
  if (forward) {
    if (fn(view)) {
      return true;
    }

    auto children = view.Children();
    for (auto it = children.begin(); it != children.end(); ++it) {
      return walkTree(*it, forward, fn);
    }

    auto current = view;
    auto parent = current.Parent();
    while (parent) {
      auto parentsChildren = parent.Children();
      auto itNextView = std::find(parentsChildren.begin(), parentsChildren.end(), current);
      assert(itNextView != parentsChildren.end());
      ++itNextView;
      if (itNextView != parentsChildren.end()) {
        return walkTree(*itNextView, true, fn);
      }
      current = parent;
      parent = current.Parent();
    }

  } else {
    auto current = view;
    auto parent = current.Parent();
    while (parent) {
      auto parentsChildren = parent.Children();
      uint32_t index;
      bool success = parent.Children().IndexOf(current, index);
      assert(success);
      if (index > 0) {
        auto lastChild = lastDeepChild(parent.Children().GetAt(index - 1));
        if (fn(lastChild))
          return true;
        return walkTree(lastChild, false, fn);
      }

      if (fn(parent)) {
        return true;
      }
      current = parent;
      parent = current.Parent();
    }
  }
  return false;
}

} // namespace winrt::Microsoft::ReactNative::implementation
