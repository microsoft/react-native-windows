// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>

// Mock implementations of the Composition.Experimental interfaces so that
// Fabric can run in headless (ui-less) integration tests.  Every method is a
// no-op and every property returns a sensible default.

namespace MockComposition {

namespace Exp = winrt::Microsoft::ReactNative::Composition::Experimental;

// ---------- IBrush / IDrawingSurfaceBrush ----------

struct MockBrush : winrt::implements<MockBrush, Exp::IBrush> {};

struct MockDrawingSurfaceBrush : winrt::implements<MockDrawingSurfaceBrush, Exp::IDrawingSurfaceBrush, Exp::IBrush> {
  void HorizontalAlignmentRatio(float) {}
  void VerticalAlignmentRatio(float) {}
  void Stretch(Exp::CompositionStretch) {}
};

// ---------- IDropShadow ----------

struct MockDropShadow : winrt::implements<MockDropShadow, Exp::IDropShadow> {
  void Offset(winrt::Windows::Foundation::Numerics::float3) {}
  void Opacity(float) {}
  void BlurRadius(float) {}
  void Color(winrt::Windows::UI::Color) {}
  void Mask(Exp::IBrush) {}
  void SourcePolicy(Exp::CompositionDropShadowSourcePolicy) {}
};

// ---------- IVisual (shared base for visual mocks) ----------

template <typename D, typename... I>
struct MockVisualBase : winrt::implements<D, I...> {
  void InsertAt(Exp::IVisual, int32_t) {}
  void Remove(Exp::IVisual) {}
  Exp::IVisual GetAt(uint32_t) { return nullptr; }
  void Opacity(float) {}
  void Scale(winrt::Windows::Foundation::Numerics::float3) {}
  void TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4) {}
  void RotationAngle(float) {}
  void IsVisible(bool) {}
  void Size(winrt::Windows::Foundation::Numerics::float2) {}
  void Offset(winrt::Windows::Foundation::Numerics::float3) {}
  void Offset(winrt::Windows::Foundation::Numerics::float3, winrt::Windows::Foundation::Numerics::float3) {}
  void RelativeSizeWithOffset(
      winrt::Windows::Foundation::Numerics::float2,
      winrt::Windows::Foundation::Numerics::float2) {}
  Exp::BackfaceVisibility BackfaceVisibility() { return Exp::BackfaceVisibility::Visible; }
  void BackfaceVisibility(Exp::BackfaceVisibility) {}
  winrt::hstring Comment() { return {}; }
  void Comment(winrt::hstring const &) {}
  void AnimationClass(Exp::AnimationClass) {}
};

// ---------- ISpriteVisual ----------

struct MockSpriteVisual : MockVisualBase<MockSpriteVisual, Exp::ISpriteVisual, Exp::IVisual> {
  void Brush(Exp::IBrush) {}
  void Shadow(Exp::IDropShadow) {}
};

// ---------- IRoundedRectangleVisual ----------

struct MockRoundedRectangleVisual
    : MockVisualBase<MockRoundedRectangleVisual, Exp::IRoundedRectangleVisual, Exp::IVisual> {
  void Brush(Exp::IBrush) {}
  void CornerRadius(winrt::Windows::Foundation::Numerics::float2) {}
  void StrokeBrush(Exp::IBrush) {}
  void StrokeThickness(float) {}
};

// ---------- IScrollVisual ----------

struct MockScrollVisual : MockVisualBase<MockScrollVisual, Exp::IScrollVisual, Exp::IVisual> {
  void Brush(Exp::IBrush) {}
  void ScrollEnabled(bool) {}
  winrt::event_token ScrollPositionChanged(
      winrt::Windows::Foundation::EventHandler<Exp::IScrollPositionChangedArgs> const &) {
    return {};
  }
  void ScrollPositionChanged(winrt::event_token) {}
  winrt::event_token ScrollBeginDrag(
      winrt::Windows::Foundation::EventHandler<Exp::IScrollPositionChangedArgs> const &) {
    return {};
  }
  void ScrollBeginDrag(winrt::event_token) {}
  winrt::event_token ScrollEndDrag(
      winrt::Windows::Foundation::EventHandler<Exp::IScrollPositionChangedArgs> const &) {
    return {};
  }
  void ScrollEndDrag(winrt::event_token) {}
  winrt::event_token ScrollMomentumBegin(
      winrt::Windows::Foundation::EventHandler<Exp::IScrollPositionChangedArgs> const &) {
    return {};
  }
  void ScrollMomentumBegin(winrt::event_token) {}
  winrt::event_token ScrollMomentumEnd(
      winrt::Windows::Foundation::EventHandler<Exp::IScrollPositionChangedArgs> const &) {
    return {};
  }
  void ScrollMomentumEnd(winrt::event_token) {}
  void ContentSize(winrt::Windows::Foundation::Numerics::float2) {}
  winrt::Windows::Foundation::Numerics::float3 ScrollPosition() { return {}; }
  void ScrollBy(winrt::Windows::Foundation::Numerics::float3, bool) {}
  void TryUpdatePosition(winrt::Windows::Foundation::Numerics::float3, bool) {}
  void OnPointerPressed(winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs const &) {}
  void SetDecelerationRate(winrt::Windows::Foundation::Numerics::float3) {}
  void SetMaximumZoomScale(float) {}
  void SetMinimumZoomScale(float) {}
  bool Horizontal() { return false; }
  void Horizontal(bool) {}
  void SetSnapPoints(bool, bool, winrt::Windows::Foundation::Collections::IVectorView<float> const &) {}
  void PagingEnabled(bool) {}
  void SnapToInterval(float) {}
  void SnapToAlignment(Exp::SnapPointsAlignment) {}
};

// ---------- IActivityVisual ----------

struct MockActivityVisual : MockVisualBase<MockActivityVisual, Exp::IActivityVisual, Exp::IVisual> {
  using MockVisualBase::Size; // IVisual::Size(float2)
  void Size(float) {}         // IActivityVisual::Size(float)
  void Brush(Exp::IBrush) {}
  void StartAnimation() {}
  void StopAnimation() {}
};

// ---------- ICaretVisual ----------

struct MockCaretVisual : winrt::implements<MockCaretVisual, Exp::ICaretVisual> {
  Exp::IVisual InnerVisual() { return winrt::make<MockSpriteVisual>(); }
  void Size(winrt::Windows::Foundation::Numerics::float2) {}
  void Position(winrt::Windows::Foundation::Numerics::float2) {}
  bool IsVisible() { return false; }
  void IsVisible(bool) {}
  void Brush(Exp::IBrush) {}
};

// ---------- IFocusVisual ----------

struct MockFocusVisual : winrt::implements<MockFocusVisual, Exp::IFocusVisual> {
  Exp::IVisual InnerVisual() { return winrt::make<MockSpriteVisual>(); }
  bool IsFocused() { return false; }
  void IsFocused(bool) {}
  float ScaleFactor() { return 1.0f; }
  void ScaleFactor(float) {}
};

// ---------- ICompositionContext ----------

struct MockCompositionContext
    : winrt::implements<MockCompositionContext, Exp::ICompositionContext> {
  Exp::ISpriteVisual CreateSpriteVisual() { return winrt::make<MockSpriteVisual>(); }
  Exp::IScrollVisual CreateScrollerVisual() { return winrt::make<MockScrollVisual>(); }
  Exp::IRoundedRectangleVisual CreateRoundedRectangleVisual() {
    return winrt::make<MockRoundedRectangleVisual>();
  }
  Exp::IActivityVisual CreateActivityVisual() { return winrt::make<MockActivityVisual>(); }
  Exp::ICaretVisual CreateCaretVisual() { return winrt::make<MockCaretVisual>(); }
  Exp::IFocusVisual CreateFocusVisual() { return winrt::make<MockFocusVisual>(); }
  Exp::IDropShadow CreateDropShadow() { return winrt::make<MockDropShadow>(); }
  Exp::IBrush CreateColorBrush(winrt::Windows::UI::Color) { return winrt::make<MockBrush>(); }
  Exp::IDrawingSurfaceBrush CreateDrawingSurfaceBrush(
      winrt::Windows::Foundation::Size,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode) {
    return winrt::make<MockDrawingSurfaceBrush>();
  }
};

} // namespace MockComposition
