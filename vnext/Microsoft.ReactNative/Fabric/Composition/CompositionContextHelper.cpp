
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.Experimental.SystemCompositionContextHelper.g.cpp")
#include "Composition.Experimental.SystemCompositionContextHelper.g.cpp"
#endif
#ifdef USE_WINUI3
#if __has_include("Composition.Experimental.MicrosoftCompositionContextHelper.g.cpp")
#include "Composition.Experimental.MicrosoftCompositionContextHelper.g.cpp"
#endif
#endif

#include <Windows.Graphics.Interop.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.interactions.h>
#include "CompositionHelpers.h"

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Composition.interactions.h>
#include <winrt/Microsoft.UI.Composition.interop.h>
#endif

namespace Microsoft::ReactNative::Composition::Experimental {

template <typename TSpriteVisual>
struct CompositionTypeTraits {};

struct WindowsTypeTag;
template <>
struct CompositionTypeTraits<WindowsTypeTag> {
  using AnimationDelayBehavior = winrt::Windows::UI::Composition::AnimationDelayBehavior;
  using AnimationDirection = winrt::Windows::UI::Composition::AnimationDirection;
  using AnimationIterationBehavior = winrt::Windows::UI::Composition::AnimationIterationBehavior;
  using CompositionAnimation = winrt::Windows::UI::Composition::CompositionAnimation;
  using CompositionBackfaceVisibility = winrt::Windows::UI::Composition::CompositionBackfaceVisibility;
  using CompositionBrush = winrt::Windows::UI::Composition::CompositionBrush;
  using CompositionColorBrush = winrt::Windows::UI::Composition::CompositionColorBrush;
  using CompositionContainerShape = winrt::Windows::UI::Composition::CompositionContainerShape;
  using CompositionDrawingSurface = winrt::Windows::UI::Composition::CompositionDrawingSurface;
  using CompositionEasingFunction = winrt::Windows::UI::Composition::CompositionEasingFunction;
  using CompositionEllipseGeometry = winrt::Windows::UI::Composition::CompositionEllipseGeometry;
  using CompositionGeometry = winrt::Windows::UI::Composition::CompositionGeometry;
  using CompositionRoundedRectangleGeometry = winrt::Windows::UI::Composition::CompositionRoundedRectangleGeometry;
  using CompositionNineGridBrush = winrt::Windows::UI::Composition::CompositionNineGridBrush;
  using CompositionObject = winrt::Windows::UI::Composition::CompositionObject;
  using CompositionPath = winrt::Windows::UI::Composition::CompositionPath;
  using CompositionPropertySet = winrt::Windows::UI::Composition::CompositionPropertySet;
  using CompositionSpriteShape = winrt::Windows::UI::Composition::CompositionSpriteShape;
  using CompositionStretch = winrt::Windows::UI::Composition::CompositionStretch;
  using CompositionStrokeCap = winrt::Windows::UI::Composition::CompositionStrokeCap;
  using CompositionSurfaceBrush = winrt::Windows::UI::Composition::CompositionSurfaceBrush;
  using Compositor = winrt::Windows::UI::Composition::Compositor;
  using ContainerVisual = winrt::Windows::UI::Composition::ContainerVisual;
  using CubicBezierEasingFunction = winrt::Windows::UI::Composition::CubicBezierEasingFunction;
  using DropShadow = winrt::Windows::UI::Composition::DropShadow;
  using ExpressionAnimation = winrt::Windows::UI::Composition::ExpressionAnimation;
  using ICompositionSurface = winrt::Windows::UI::Composition::ICompositionSurface;
  using IInteractionTrackerOwner = winrt::Windows::UI::Composition::Interactions::IInteractionTrackerOwner;
  using InteractionSourceMode = winrt::Windows::UI::Composition::Interactions::InteractionSourceMode;
  using InteractionTracker = winrt::Windows::UI::Composition::Interactions::InteractionTracker;
  using InteractionTrackerCustomAnimationStateEnteredArgs =
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs;
  using InteractionTrackerIdleStateEnteredArgs =
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs;
  using InteractionTrackerInertiaStateEnteredArgs =
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs;
  using InteractionTrackerInteractingStateEnteredArgs =
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs;
  using InteractionTrackerRequestIgnoredArgs =
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs;
  using InteractionTrackerValuesChangedArgs =
      winrt::Windows::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs;
  using ScalarKeyFrameAnimation = winrt::Windows::UI::Composition::ScalarKeyFrameAnimation;
  using ShapeVisual = winrt::Windows::UI::Composition::ShapeVisual;
  using SpriteVisual = winrt::Windows::UI::Composition::SpriteVisual;
  using StepEasingFunction = winrt::Windows::UI::Composition::StepEasingFunction;
  using Visual = winrt::Windows::UI::Composition::Visual;
  using VisualInteractionSource = winrt::Windows::UI::Composition::Interactions::VisualInteractionSource;
  using VisualInteractionSourceRedirectionMode =
      winrt::Windows::UI::Composition::Interactions::VisualInteractionSourceRedirectionMode;
  using CompositionGraphicsDevice = winrt::Windows::UI::Composition::CompositionGraphicsDevice;

  using ICompositionDrawingSurfaceInterop = ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop;
  using ICompositorInterop = ABI::Windows::UI::Composition::ICompositorInterop;

  using IInnerCompositionCompositor = IWindowsCompositionCompositor;
  using IInnerCompositionDropShadow = IWindowsCompositionDropShadow;
  using IInnerCompositionVisual = IWindowsCompositionVisual;
  using IInnerCompositionBrush = IWindowsCompositionBrush;
  using IInnerCompositionDrawingSurface = IWindowsCompositionDrawingSurfaceInner;
  using CompositionContextHelper =
      winrt::Microsoft::ReactNative::Composition::Experimental::SystemCompositionContextHelper;
};
using WindowsTypeRedirects = CompositionTypeTraits<WindowsTypeTag>;

#ifdef USE_WINUI3
struct MicrosoftTypeTag;
template <>
struct CompositionTypeTraits<MicrosoftTypeTag> {
  using AnimationDelayBehavior = winrt::Microsoft::UI::Composition::AnimationDelayBehavior;
  using AnimationDirection = winrt::Microsoft::UI::Composition::AnimationDirection;
  using AnimationIterationBehavior = winrt::Microsoft::UI::Composition::AnimationIterationBehavior;
  using CompositionAnimation = winrt::Microsoft::UI::Composition::CompositionAnimation;
  using CompositionBackfaceVisibility = winrt::Microsoft::UI::Composition::CompositionBackfaceVisibility;
  using CompositionBrush = winrt::Microsoft::UI::Composition::CompositionBrush;
  using CompositionColorBrush = winrt::Microsoft::UI::Composition::CompositionColorBrush;
  using CompositionContainerShape = winrt::Microsoft::UI::Composition::CompositionContainerShape;
  using CompositionDrawingSurface = winrt::Microsoft::UI::Composition::CompositionDrawingSurface;
  using CompositionEasingFunction = winrt::Microsoft::UI::Composition::CompositionEasingFunction;
  using CompositionEllipseGeometry = winrt::Microsoft::UI::Composition::CompositionEllipseGeometry;
  using CompositionGeometry = winrt::Microsoft::UI::Composition::CompositionGeometry;
  using CompositionRoundedRectangleGeometry = winrt::Microsoft::UI::Composition::CompositionRoundedRectangleGeometry;
  using CompositionNineGridBrush = winrt::Microsoft::UI::Composition::CompositionNineGridBrush;
  using CompositionObject = winrt::Microsoft::UI::Composition::CompositionObject;
  using CompositionPath = winrt::Microsoft::UI::Composition::CompositionPath;
  using CompositionPropertySet = winrt::Microsoft::UI::Composition::CompositionPropertySet;
  using CompositionSpriteShape = winrt::Microsoft::UI::Composition::CompositionSpriteShape;
  using CompositionStretch = winrt::Microsoft::UI::Composition::CompositionStretch;
  using CompositionStrokeCap = winrt::Microsoft::UI::Composition::CompositionStrokeCap;
  using CompositionSurfaceBrush = winrt::Microsoft::UI::Composition::CompositionSurfaceBrush;
  using Compositor = winrt::Microsoft::UI::Composition::Compositor;
  using ContainerVisual = winrt::Microsoft::UI::Composition::ContainerVisual;
  using CubicBezierEasingFunction = winrt::Microsoft::UI::Composition::CubicBezierEasingFunction;
  using DropShadow = winrt::Microsoft::UI::Composition::DropShadow;
  using ExpressionAnimation = winrt::Microsoft::UI::Composition::ExpressionAnimation;
  using ICompositionSurface = winrt::Microsoft::UI::Composition::ICompositionSurface;
  using IInteractionTrackerOwner = winrt::Microsoft::UI::Composition::Interactions::IInteractionTrackerOwner;
  using InteractionSourceMode = winrt::Microsoft::UI::Composition::Interactions::InteractionSourceMode;
  using InteractionTracker = winrt::Microsoft::UI::Composition::Interactions::InteractionTracker;
  using InteractionTrackerCustomAnimationStateEnteredArgs =
      winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs;
  using InteractionTrackerIdleStateEnteredArgs =
      winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs;
  using InteractionTrackerInertiaStateEnteredArgs =
      winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs;
  using InteractionTrackerInteractingStateEnteredArgs =
      winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs;
  using InteractionTrackerRequestIgnoredArgs =
      winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs;
  using InteractionTrackerValuesChangedArgs =
      winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs;
  using ScalarKeyFrameAnimation = winrt::Microsoft::UI::Composition::ScalarKeyFrameAnimation;
  using ShapeVisual = winrt::Microsoft::UI::Composition::ShapeVisual;
  using SpriteVisual = winrt::Microsoft::UI::Composition::SpriteVisual;
  using StepEasingFunction = winrt::Microsoft::UI::Composition::StepEasingFunction;
  using Visual = winrt::Microsoft::UI::Composition::Visual;
  using VisualInteractionSource = winrt::Microsoft::UI::Composition::Interactions::VisualInteractionSource;
  using VisualInteractionSourceRedirectionMode =
      winrt::Microsoft::UI::Composition::Interactions::VisualInteractionSourceRedirectionMode;
  using CompositionGraphicsDevice = winrt::Microsoft::UI::Composition::CompositionGraphicsDevice;

  using ICompositionDrawingSurfaceInterop = winrt::Microsoft::UI::Composition::ICompositionDrawingSurfaceInterop;
  using ICompositorInterop = winrt::Microsoft::UI::Composition::ICompositorInterop;

  using IInnerCompositionCompositor = IMicrosoftCompositionCompositor;
  using IInnerCompositionDropShadow = IMicrosoftCompositionDropShadow;
  using IInnerCompositionVisual = IMicrosoftCompositionVisual;
  using IInnerCompositionBrush = IMicrosoftCompositionBrush;
  using IInnerCompositionDrawingSurface = IMicrosoftCompositionDrawingSurfaceInner;
  using CompositionContextHelper =
      winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper;
};
using MicrosoftTypeRedirects = CompositionTypeTraits<MicrosoftTypeTag>;
#endif

struct GeometrySource : public winrt::implements<
                            GeometrySource,
                            winrt::Windows::Graphics::IGeometrySource2D,
                            ABI::Windows::Graphics::IGeometrySource2DInterop> {
  GeometrySource(ID2D1Geometry *pGeometry) noexcept {
    m_geometry.copy_from(pGeometry);
  }

  IFACEMETHODIMP GetGeometry(ID2D1Geometry **value) noexcept override {
    m_geometry.copy_to(value);
    return S_OK;
  }

  IFACEMETHODIMP TryGetGeometryUsingFactory(ID2D1Factory *, ID2D1Geometry **) noexcept override {
    return E_NOTIMPL;
  }

 private:
  winrt::com_ptr<ID2D1Geometry> m_geometry;
};

template <typename TTypeRedirects>
struct CompDropShadow : public winrt::implements<
                            CompDropShadow<TTypeRedirects>,
                            winrt::Microsoft::ReactNative::Composition::Experimental::IDropShadow,
                            typename TTypeRedirects::IInnerCompositionDropShadow> {
  CompDropShadow(typename TTypeRedirects::DropShadow const &shadow) : m_shadow(shadow) {}

  typename TTypeRedirects::DropShadow InnerShadow() const noexcept override {
    return m_shadow;
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept {
    m_shadow.Offset(offset);
  }

  void Opacity(float opacity) noexcept {
    m_shadow.Opacity(opacity);
  }

  void BlurRadius(float radius) noexcept {
    m_shadow.BlurRadius(radius);
  }

  void Color(winrt::Windows::UI::Color color) noexcept {
    m_shadow.Color(color);
  }

 private:
  typename TTypeRedirects::DropShadow m_shadow;
};
using WindowsCompDropShadow = CompDropShadow<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompDropShadow = CompDropShadow<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompBrush : public winrt::implements<
                       CompBrush<TTypeRedirects>,
                       winrt::Microsoft::ReactNative::Composition::Experimental::IBrush,
                       typename TTypeRedirects::IInnerCompositionBrush> {
  CompBrush(typename TTypeRedirects::CompositionBrush const &brush) : m_brush(brush) {}

  typename TTypeRedirects::CompositionBrush InnerBrush() const noexcept {
    return m_brush;
  }

 private:
  typename TTypeRedirects::CompositionBrush m_brush;
};
using WindowsCompBrush = CompBrush<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompBrush = CompBrush<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompDrawingSurfaceBrush : public winrt::implements<
                                     CompDrawingSurfaceBrush<TTypeRedirects>,
                                     winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush,
                                     winrt::Microsoft::ReactNative::Composition::Experimental::IBrush,
                                     typename TTypeRedirects::IInnerCompositionBrush,
                                     ICompositionDrawingSurfaceInterop,
                                     typename TTypeRedirects::IInnerCompositionDrawingSurface> {
  CompDrawingSurfaceBrush(
      const typename TTypeRedirects::Compositor &compositor,
      typename TTypeRedirects::CompositionDrawingSurface const &drawingSurface)
      : m_brush(compositor.CreateSurfaceBrush(drawingSurface)) {
    drawingSurface.as(m_drawingSurfaceInterop);
  }

  HRESULT BeginDraw(ID2D1DeviceContext **deviceContextOut, float xDpi, float yDpi, POINT *offset) noexcept {
#ifdef DEBUG
    // Drawing to a zero sized surface is a waste of time
    auto size = m_drawingSurfaceInterop.as<typename TTypeRedirects::CompositionDrawingSurface>().Size();
    assert(size.Width != 0 && size.Height != 0);
#endif

    auto hr =
        m_drawingSurfaceInterop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), (void **)deviceContextOut, offset);
    if (SUCCEEDED(hr)) {
      (*deviceContextOut)->SetDpi(xDpi, yDpi);
    }
    return hr;
  }

  HRESULT EndDraw() noexcept {
    return m_drawingSurfaceInterop->EndDraw();
  }

  typename TTypeRedirects::ICompositionSurface Inner() const noexcept {
    typename TTypeRedirects::ICompositionSurface surface;
    m_drawingSurfaceInterop.as(surface);
    return surface;
  }

  typename TTypeRedirects::CompositionBrush InnerBrush() const noexcept {
    return m_brush;
  }

  void HorizontalAlignmentRatio(float ratio) noexcept {
    m_brush.HorizontalAlignmentRatio(ratio);
  }
  void VerticalAlignmentRatio(float ratio) noexcept {
    m_brush.VerticalAlignmentRatio(ratio);
  }

  void Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch mode) noexcept {
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch>(
            TTypeRedirects::CompositionStretch::None) ==
        winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch>(
            TTypeRedirects::CompositionStretch::Fill) ==
        winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::Fill);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch>(
            TTypeRedirects::CompositionStretch::Uniform) ==
        winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::Uniform);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch>(
            TTypeRedirects::CompositionStretch::UniformToFill) ==
        winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::UniformToFill);

    m_brush.Stretch(static_cast<typename TTypeRedirects::CompositionStretch>(mode));
  }

 private:
  typename TTypeRedirects::CompositionSurfaceBrush m_brush;
  winrt::com_ptr<typename TTypeRedirects::ICompositionDrawingSurfaceInterop> m_drawingSurfaceInterop;
};
using WindowsCompDrawingSurfaceBrush = CompDrawingSurfaceBrush<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompDrawingSurfaceBrush = CompDrawingSurfaceBrush<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
void SetAnimationClass(
    winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass value,
    typename const TTypeRedirects::Visual &visual) {
  constexpr int64_t ScrollBarExpandBeginTime = 400; // ms
  constexpr int64_t ScrollBarExpandDuration = 167; // ms
  constexpr int64_t SwitchDuration = 167; // ms

  switch (value) {
    case winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::None: {
      visual.ImplicitAnimations(nullptr);
      break;
    }

    case winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::SwitchThumb: {
      auto compositor = visual.Compositor();
      auto offsetAnimation = compositor.CreateVector3KeyFrameAnimation();
      offsetAnimation.InsertExpressionKeyFrame(
          0.0f, L"vector3(this.CurrentValue.X, this.FinalValue.Y, this.FinalValue.Z)");
      offsetAnimation.InsertExpressionKeyFrame(1.0f, L"this.FinalValue");
      offsetAnimation.Target(L"Offset");
      offsetAnimation.Duration(std::chrono::milliseconds(SwitchDuration));

      auto implicitAnimations = compositor.CreateImplicitAnimationCollection();
      implicitAnimations.Insert(L"Offset", offsetAnimation);

      visual.ImplicitAnimations(implicitAnimations);
      break;
    }

    case winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBar: {
      auto compositor = visual.Compositor();

      auto opacityAnimation = compositor.CreateScalarKeyFrameAnimation();
      opacityAnimation.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
      opacityAnimation.InsertExpressionKeyFrame(1.0f, L"this.FinalValue");
      opacityAnimation.Target(L"Opacity");
      opacityAnimation.DelayTime(std::chrono::milliseconds(ScrollBarExpandBeginTime));
      opacityAnimation.Duration(std::chrono::milliseconds(ScrollBarExpandDuration));

      auto implicitAnimations = compositor.CreateImplicitAnimationCollection();
      implicitAnimations.Insert(L"Opacity", opacityAnimation);
      visual.ImplicitAnimations(implicitAnimations);
      break;
    }

    case winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBarThumbVertical:
    case winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBarThumbHorizontal: {
      auto compositor = visual.Compositor();
      auto offsetAnimation = compositor.CreateVector3KeyFrameAnimation();
      // We cannot just use a DelayTime, since we want changes to the offset in the scrolling dirction to happen
      // immediately, and only delay the offset changes used when hiding/showing the scrollbar
      float delayStartFrameOffset =
          static_cast<float>(ScrollBarExpandBeginTime) / (ScrollBarExpandBeginTime + ScrollBarExpandDuration);
      if (value == winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBarThumbVertical) {
        offsetAnimation.InsertExpressionKeyFrame(
            0.0f, L"vector3(this.CurrentValue.X, this.FinalValue.Y, this.FinalValue.Z)");
        offsetAnimation.InsertExpressionKeyFrame(
            delayStartFrameOffset, L"vector3(this.CurrentValue.X, this.FinalValue.Y, this.FinalValue.Z)");
      } else {
        offsetAnimation.InsertExpressionKeyFrame(
            0.0f, L"vector3(this.FinalValue.X, this.CurrentValue.Y, this.FinalValue.Z)");
        offsetAnimation.InsertExpressionKeyFrame(
            delayStartFrameOffset, L"vector3(this.FinalValue.X, this.CurrentValue.Y, this.FinalValue.Z)");
      }
      offsetAnimation.InsertExpressionKeyFrame(1.0f, L"this.FinalValue");
      offsetAnimation.Target(L"Offset");
      offsetAnimation.Duration(std::chrono::milliseconds(ScrollBarExpandBeginTime + ScrollBarExpandDuration));

      auto scaleAnimation = compositor.CreateVector3KeyFrameAnimation();
      scaleAnimation.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
      scaleAnimation.InsertExpressionKeyFrame(1.0f, L"this.FinalValue");
      scaleAnimation.Target(L"Scale");
      scaleAnimation.DelayTime(std::chrono::milliseconds(ScrollBarExpandBeginTime));
      scaleAnimation.Duration(std::chrono::milliseconds(ScrollBarExpandDuration));

      auto implicitAnimations = compositor.CreateImplicitAnimationCollection();
      implicitAnimations.Insert(L"Offset", offsetAnimation);
      implicitAnimations.Insert(L"Scale", scaleAnimation);

      visual.ImplicitAnimations(implicitAnimations);
    }
  }
}

template <typename TTypeRedirects, typename TVisual = typename TTypeRedirects::Visual>
struct CompVisualImpl {
  CompVisualImpl(typename TVisual const &visual) : m_visual(visual) {}

  typename TTypeRedirects::Visual InnerVisual() const noexcept {
    return m_visual;
  }

  void InsertAt(
      const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual,
      uint32_t index) noexcept {
    auto containerChildren = InnerVisual().as<typename TTypeRedirects::ContainerVisual>().Children();
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    if (index == 0) {
      containerChildren.InsertAtBottom(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = InnerVisual().as<typename TTypeRedirects::ContainerVisual>().Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<typename TTypeRedirects::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return TTypeRedirects::CompositionContextHelper::CreateVisual(it.Current());
  }

  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept {
    if (!clippingPath) {
      m_visual.Clip(nullptr);
      return;
    }
    auto geometry = winrt::make<GeometrySource>(clippingPath);
    auto path = typename TTypeRedirects::CompositionPath(geometry);
    auto pathgeo = m_visual.Compositor().CreatePathGeometry(path);
    auto clip = m_visual.Compositor().CreateGeometricClip(pathgeo);
    m_visual.Clip(clip);
  }

  void Opacity(float opacity) noexcept {
    m_visual.Opacity(opacity);
  }

  void Scale(winrt::Windows::Foundation::Numerics::float3 const &scale) noexcept {
    m_visual.Scale(scale);
  }

  void TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4 const &transform) noexcept {
    m_visual.TransformMatrix(transform);
  }

  void RotationAngle(float rotation) noexcept {
    m_visual.RotationAngle(rotation);
  }

  void IsVisible(bool isVisible) noexcept {
    m_visual.IsVisible(isVisible);
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept {
    m_visual.Size(size);
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept {
    m_visual.Offset(offset);
  }

  void Offset(
      winrt::Windows::Foundation::Numerics::float3 offset,
      winrt::Windows::Foundation::Numerics::float3 relativeAdjustment) noexcept {
    m_visual.Offset(offset);
    m_visual.RelativeOffsetAdjustment(relativeAdjustment);
  }

  void RelativeSizeWithOffset(
      winrt::Windows::Foundation::Numerics::float2 size,
      winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment) noexcept {
    m_visual.Size(size);
    m_visual.RelativeSizeAdjustment(relativeSizeAdjustment);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility BackfaceVisibility() const noexcept {
    return static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility>(
        m_visual.BackfaceVisibility());
  }

  void BackfaceVisibility(winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility value) noexcept {
    m_visual.BackfaceVisibility(static_cast<typename TTypeRedirects::CompositionBackfaceVisibility>(value));
  }

  winrt::hstring Comment() const noexcept {
    return m_visual.Comment();
  }

  void Comment(winrt::hstring value) noexcept {
    m_visual.Comment(value);
  }

  void AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass value) noexcept {
    SetAnimationClass<TTypeRedirects>(value, m_visual);
  }

 protected:
  TVisual m_visual;
};

template <typename TTypeRedirects>
struct CompVisual : public winrt::implements<
                        CompVisual<TTypeRedirects>,
                        winrt::Microsoft::ReactNative::Composition::Experimental::IVisual,
                        typename TTypeRedirects::IInnerCompositionVisual,
                        IVisualInterop>,
                    CompVisualImpl<TTypeRedirects> {
  using Super = CompVisualImpl<TTypeRedirects>;
  CompVisual(typename TTypeRedirects::Visual const &visual) : CompVisualImpl<TTypeRedirects>(visual) {}

  // IInnerCompositionVisual
  typename TTypeRedirects::Visual InnerVisual() const noexcept override {
    return Super::m_visual;
  }

  // IVisualInterop
  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept override {
    Super::SetClippingPath(clippingPath);
  }
};
using WindowsCompVisual = CompVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompVisual = CompVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompSpriteVisual : winrt::implements<
                              CompSpriteVisual<TTypeRedirects>,
                              winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual,
                              winrt::Microsoft::ReactNative::Composition::Experimental::IVisual,
                              typename TTypeRedirects::IInnerCompositionVisual,
                              IVisualInterop>,
                          CompVisualImpl<TTypeRedirects, typename TTypeRedirects::SpriteVisual> {
  using Super = CompVisualImpl<TTypeRedirects, typename TTypeRedirects::SpriteVisual>;
  CompSpriteVisual(typename TTypeRedirects::SpriteVisual const &visual) : Super(visual) {}

  // IInnerCompositionVisual
  typename TTypeRedirects::Visual InnerVisual() const noexcept override {
    return Super::m_visual;
  }

  // IVisualInterop
  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept override {
    Super::SetClippingPath(clippingPath);
  }

  void Brush(const winrt::Microsoft::ReactNative::Composition::Experimental::IBrush &brush) noexcept {
    Super::m_visual.Brush(TTypeRedirects::CompositionContextHelper::InnerBrush(brush));
  }

  void Shadow(const winrt::Microsoft::ReactNative::Composition::Experimental::IDropShadow &shadow) noexcept {
    Super::m_visual.Shadow(TTypeRedirects::CompositionContextHelper::InnerDropShadow(shadow));
  }
};
using WindowsCompSpriteVisual = CompSpriteVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompSpriteVisual = CompSpriteVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompRoundedRectangleVisual
    : winrt::implements<
          CompRoundedRectangleVisual<TTypeRedirects>,
          winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual,
          winrt::Microsoft::ReactNative::Composition::Experimental::IVisual,
          typename TTypeRedirects::IInnerCompositionVisual,
          IVisualInterop>,
      CompVisualImpl<TTypeRedirects> {
  using Super = CompVisualImpl<TTypeRedirects>;
  CompRoundedRectangleVisual(typename TTypeRedirects::ShapeVisual const &visual) : Super(visual) {
    auto compositor = visual.Compositor();
    m_geometry = compositor.CreateRoundedRectangleGeometry();
    m_spriteShape = compositor.CreateSpriteShape();
    m_spriteShape.Geometry(m_geometry);
    visual.Shapes().Append(m_spriteShape);
  }

  // IInnerCompositionVisual
  typename TTypeRedirects::Visual InnerVisual() const noexcept override {
    return Super::m_visual;
  }

  // IVisualInterop
  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept override {
    Super::SetClippingPath(clippingPath);
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept {
    m_size = size;
    Super::m_visual.Size(size);
    updateGeometry();
  }

  void updateGeometry() {
    m_geometry.Size({m_size.x - (m_strokeThickness * 2), m_size.y - (m_strokeThickness * 2)});
    m_geometry.CornerRadius({m_cornerRadius.x - m_strokeThickness, m_cornerRadius.y - m_strokeThickness});
    m_geometry.Offset({m_strokeThickness, m_strokeThickness});
  }

  void RelativeSizeWithOffset(
      winrt::Windows::Foundation::Numerics::float2 size,
      winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment) noexcept {
    assert(false); // Does not correctly handle relativeSizeAdjustment - since geometry does not support
                   // RelativeSizeAdjustment
    m_size = size;
    Super::m_visual.Size(size);
    updateGeometry();

    Super::m_visual.RelativeSizeAdjustment(relativeSizeAdjustment);
  }

  void Brush(const winrt::Microsoft::ReactNative::Composition::Experimental::IBrush &brush) noexcept {
    m_spriteShape.FillBrush(TTypeRedirects::CompositionContextHelper::InnerBrush(brush));
  }

  void CornerRadius(winrt::Windows::Foundation::Numerics::float2 value) noexcept {
    m_cornerRadius = value;
    updateGeometry();
  }

  void StrokeBrush(const winrt::Microsoft::ReactNative::Composition::Experimental::IBrush &brush) noexcept {
    m_spriteShape.StrokeBrush(TTypeRedirects::CompositionContextHelper::InnerBrush(brush));
  }

  void StrokeThickness(float value) noexcept {
    m_strokeThickness = value;
    m_spriteShape.StrokeThickness(value);
    updateGeometry();
  }

 private:
  float m_strokeThickness{0.0f};
  winrt::Windows::Foundation::Numerics::float2 m_cornerRadius{0};
  winrt::Windows::Foundation::Numerics::float2 m_size{0};
  typename TTypeRedirects::CompositionSpriteShape m_spriteShape{nullptr};
  typename TTypeRedirects::CompositionRoundedRectangleGeometry m_geometry{nullptr};
};
using WindowsCompRoundedRectangleVisual = CompRoundedRectangleVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompRoundedRectangleVisual = CompRoundedRectangleVisual<MicrosoftTypeRedirects>;
#endif

struct CompScrollPositionChangedArgs
    : winrt::implements<
          CompScrollPositionChangedArgs,
          winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs> {
  CompScrollPositionChangedArgs(winrt::Windows::Foundation::Numerics::float2 position) : m_position(position) {}

  winrt::Windows::Foundation::Numerics::float2 Position() const noexcept {
    return m_position;
  }

 private:
  winrt::Windows::Foundation::Numerics::float2 m_position;
};

template <typename TTypeRedirects>
struct CompScrollerVisual : winrt::implements<
                                CompScrollerVisual<TTypeRedirects>,
                                winrt::Microsoft::ReactNative::Composition::Experimental::IScrollVisual,
                                winrt::Microsoft::ReactNative::Composition::Experimental::IVisual,
                                typename TTypeRedirects::IInnerCompositionVisual,
                                IVisualInterop> {
  struct ScrollInteractionTrackerOwner
      : public winrt::implements<ScrollInteractionTrackerOwner, typename TTypeRedirects::IInteractionTrackerOwner> {
    ScrollInteractionTrackerOwner(CompScrollerVisual *outer) : m_outer(outer){};

    void CustomAnimationStateEntered(
        typename TTypeRedirects::InteractionTracker sender,
        typename TTypeRedirects::InteractionTrackerCustomAnimationStateEnteredArgs args) noexcept {
      m_outer->m_custom = true;
      m_outer->m_inertia = false;
    }
    void IdleStateEntered(
        typename TTypeRedirects::InteractionTracker sender,
        typename TTypeRedirects::InteractionTrackerIdleStateEnteredArgs args) noexcept {
      m_outer->m_custom = false;
      m_outer->m_inertia = false;
    }
    void InertiaStateEntered(
        typename TTypeRedirects::InteractionTracker sender,
        typename TTypeRedirects::InteractionTrackerInertiaStateEnteredArgs args) noexcept {
      m_outer->m_custom = false;
      m_outer->m_inertia = true;
      m_outer->m_currentPosition = args.NaturalRestingPosition();
      // When the user stops interacting with the object, tracker can go into two paths:
      // 1. tracker goes into idle state immediately
      // 2. tracker has just started gliding into Inertia state
      // Fire ScrollEndDrag
      m_outer->FireScrollEndDrag({args.NaturalRestingPosition().x, args.NaturalRestingPosition().y});
    }
    void InteractingStateEntered(
        typename TTypeRedirects::InteractionTracker sender,
        typename TTypeRedirects::InteractionTrackerInteractingStateEnteredArgs args) noexcept {
      // Fire when the user starts dragging the object
      m_outer->FireScrollBeginDrag({sender.Position().x, sender.Position().y});
    }
    void RequestIgnored(
        typename TTypeRedirects::InteractionTracker sender,
        typename TTypeRedirects::InteractionTrackerRequestIgnoredArgs args) noexcept {}
    void ValuesChanged(
        typename TTypeRedirects::InteractionTracker sender,
        typename TTypeRedirects::InteractionTrackerValuesChangedArgs args) noexcept {
      m_outer->m_currentPosition = args.Position();
      m_outer->FireScrollPositionChanged({args.Position().x, args.Position().y});
    }

   private:
    CompScrollerVisual *m_outer;
  };

  CompScrollerVisual(typename TTypeRedirects::SpriteVisual const &visual) : m_visual(visual) {
    auto compositor = m_visual.Compositor();
    m_contentVisual = compositor.CreateSpriteVisual();

    auto brush = compositor.CreateColorBrush({0, 0, 0, 0}); // transparent brush so that hit testing works
    m_visual.Brush(brush);
    m_contentVisual.Brush(brush);

    m_visual.Children().InsertAtTop(m_contentVisual);

    m_visual.Clip(compositor.CreateInsetClip(0, 0, 0, 0));

    m_interactionTracker = TTypeRedirects::InteractionTracker::CreateWithOwner(
        compositor, winrt::make<ScrollInteractionTrackerOwner>(this));

    m_interactionTracker.MinScale(1.0);
    m_interactionTracker.MaxScale(1.0);

    m_visualInteractionSource = TTypeRedirects::VisualInteractionSource::Create(m_visual);
    m_visualInteractionSource.ScaleSourceMode(TTypeRedirects::InteractionSourceMode::Disabled);
    m_interactionTracker.InteractionSources().Add(m_visualInteractionSource);
    UpdateInteractionModes();

    auto positionExpression = compositor.CreateExpressionAnimation(L"-tracker.Position");
    positionExpression.SetReferenceParameter(L"tracker", m_interactionTracker);
    m_contentVisual.StartAnimation(L"Offset", positionExpression);
  }

  typename TTypeRedirects::Visual InnerVisual() const noexcept {
    return m_visual;
  }

  void OnPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
    if constexpr (std::is_same_v<TTypeRedirects, MicrosoftTypeRedirects>) {
      auto pointerDeviceType = args.Pointer().PointerDeviceType();
      if (pointerDeviceType == winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Touch) {
        m_visualInteractionSource.TryRedirectForManipulation(args.GetCurrentPoint(args.OriginalSource()).Inner());
      }
    }
  }

  bool Horizontal() const noexcept {
    return m_horizontal;
  }

  void Horizontal(bool value) noexcept {
    m_horizontal = value;

    UpdateInteractionModes();
  }

  void UpdateInteractionModes() noexcept {
    if (m_isScrollEnabled) {
      m_visualInteractionSource.PositionXSourceMode(
          m_horizontal ? TTypeRedirects::InteractionSourceMode::EnabledWithInertia
                       : TTypeRedirects::InteractionSourceMode::Disabled);
      m_visualInteractionSource.PositionYSourceMode(
          m_horizontal ? TTypeRedirects::InteractionSourceMode::Disabled
                       : TTypeRedirects::InteractionSourceMode::EnabledWithInertia);
      m_visualInteractionSource.ManipulationRedirectionMode(
          TTypeRedirects::VisualInteractionSourceRedirectionMode::CapableTouchpadAndPointerWheel);
    } else {
      m_visualInteractionSource.PositionXSourceMode(TTypeRedirects::InteractionSourceMode::Disabled);
      m_visualInteractionSource.PositionYSourceMode(TTypeRedirects::InteractionSourceMode::Disabled);
      m_visualInteractionSource.ManipulationRedirectionMode(
          TTypeRedirects::VisualInteractionSourceRedirectionMode::Off);
    }
  }

  void InsertAt(
      const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual,
      uint32_t index) noexcept {
    auto containerChildren = m_contentVisual.Children();
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    if (index == 0) {
      containerChildren.InsertAtBottom(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_contentVisual.Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<typename TTypeRedirects::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return TTypeRedirects::CompositionContextHelper::CreateVisual(it.Current());
  }

  void Brush(const winrt::Microsoft::ReactNative::Composition::Experimental::IBrush &brush) noexcept {
    m_visual.Brush(TTypeRedirects::CompositionContextHelper::InnerBrush(brush));
  }

  void ScrollEnabled(bool isScrollEnabled) noexcept {
    m_isScrollEnabled = isScrollEnabled;
    UpdateInteractionModes();
  }

  void SetDecelerationRate(winrt::Windows::Foundation::Numerics::float3 const &decelerationRate) noexcept {
    m_interactionTracker.PositionInertiaDecayRate(decelerationRate);
  }

  void SetMaximumZoomScale(float maximumZoomScale) const noexcept {
    m_interactionTracker.MaxScale(maximumZoomScale);
  }

  void SetMinimumZoomScale(float minimumZoomScale) noexcept {
    m_interactionTracker.MinScale(minimumZoomScale);
  }

  void SnapToStart(bool snapToStart) noexcept {
    // Store snapToStart flag for future use in scroll calculations
    // For now, this is a placeholder implementation that stores the value
    // The actual snapping logic would be implemented based on scroll calculations
    // similar to how Paper architecture handles it in SnapPointManagingContentControl
    m_snapToStart = snapToStart;
  }

  void Opacity(float opacity) noexcept {
    m_visual.Opacity(opacity);
  }

  void Scale(winrt::Windows::Foundation::Numerics::float3 const &scale) noexcept {
    m_visual.Scale(scale);
  }

  void TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4 const &transform) noexcept {
    m_visual.TransformMatrix(transform);
  }

  void RotationAngle(float rotation) noexcept {
    m_visual.RotationAngle(rotation);
  }

  void IsVisible(bool isVisible) noexcept {
    m_visual.IsVisible(isVisible);
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept {
    m_visualSize = size;
    m_visual.Size(size);
    UpdateMaxPosition();
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept {
    m_visual.Offset(offset);
  }

  void Offset(
      winrt::Windows::Foundation::Numerics::float3 offset,
      winrt::Windows::Foundation::Numerics::float3 relativeAdjustment) noexcept {
    m_visual.Offset(offset);
    m_visual.RelativeOffsetAdjustment(relativeAdjustment);
  }

  void RelativeSizeWithOffset(
      winrt::Windows::Foundation::Numerics::float2 size,
      winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment) noexcept {
    m_visual.Size(size);
    m_visual.RelativeSizeAdjustment(relativeSizeAdjustment);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility BackfaceVisibility() {
    return static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility>(
        m_visual.BackfaceVisibility());
  }

  void BackfaceVisibility(winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility value) {
    m_visual.BackfaceVisibility(static_cast<typename TTypeRedirects::CompositionBackfaceVisibility>(value));
  }

  winrt::hstring Comment() const noexcept {
    return m_visual.Comment();
  }

  void Comment(winrt::hstring value) noexcept {
    m_visual.Comment(value);
  }

  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept {
    if (!clippingPath) {
      m_visual.Clip(nullptr);
      return;
    }
    auto geometry = winrt::make<GeometrySource>(clippingPath);
    auto path = TTypeRedirects::CompositionPath(geometry);
    auto pathgeo = m_visual.Compositor().CreatePathGeometry(path);
    auto clip = m_visual.Compositor().CreateGeometricClip(pathgeo);
    m_visual.Clip(clip);
  }

  void Shadow(const winrt::Microsoft::ReactNative::Composition::Experimental::IDropShadow &shadow) noexcept {
    m_visual.Shadow(TTypeRedirects::CompositionContextHelper::InnerDropShadow(shadow));
  }

  winrt::event_token ScrollPositionChanged(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs> const
          &handler) noexcept {
    return m_scrollPositionChangedEvent.add(handler);
  }

  winrt::event_token ScrollBeginDrag(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs> const
          &handler) noexcept {
    return m_scrollBeginDragEvent.add(handler);
  }

  winrt::event_token ScrollEndDrag(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs> const
          &handler) noexcept {
    return m_scrollEndDragEvent.add(handler);
  }

  void ScrollPositionChanged(winrt::event_token const &token) noexcept {
    m_scrollPositionChangedEvent.remove(token);
  }

  void ScrollBeginDrag(winrt::event_token const &token) noexcept {
    m_scrollBeginDragEvent.remove(token);
  }

  void ScrollEndDrag(winrt::event_token const &token) noexcept {
    m_scrollEndDragEvent.remove(token);
  }

  void ContentSize(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept {
    m_contentSize = size;
    m_contentVisual.Size(size);
    UpdateMaxPosition();
  }

  winrt::Windows::Foundation::Numerics::float3 ScrollPosition() const noexcept {
    return m_interactionTracker.Position();
  }

  // ChangeOffsets scrolling constants
  static constexpr int64_t s_offsetsChangeMsPerUnit{5};
  static constexpr int64_t s_offsetsChangeMinMs{50};
  static constexpr int64_t s_offsetsChangeMaxMs{1000};

  typename TTypeRedirects::CompositionAnimation GetPositionAnimation(float x, float y) noexcept {
    const int64_t distance =
        static_cast<int64_t>(std::sqrt(std::pow(x - m_currentPosition.x, 2.0f) + pow(y - m_currentPosition.y, 2.0f)));
    auto compositor = m_visual.Compositor();
    auto positionAnimation = compositor.CreateVector3KeyFrameAnimation();

    positionAnimation.InsertKeyFrame(1.0f, {x, y, 0.0f});
    positionAnimation.Duration(std::chrono::milliseconds(
        std::clamp(distance * s_offsetsChangeMsPerUnit, s_offsetsChangeMinMs, s_offsetsChangeMaxMs)));

    return positionAnimation;
  }

  void ScrollBy(winrt::Windows::Foundation::Numerics::float3 const &offset, bool animate) noexcept {
    auto restingPosition = m_inertia ? m_interactionTracker.NaturalRestingPosition() : m_interactionTracker.Position();
    if (m_custom) {
      restingPosition = m_targetPosition;
    }
    if (animate) {
      auto maxPosition = m_interactionTracker.MaxPosition();
      m_custom = true;
      m_targetPosition = {
          std::clamp(restingPosition.x + offset.x, 0.0f, maxPosition.x),
          std::clamp(restingPosition.y + offset.y, 0.0f, maxPosition.y),
          std::clamp(restingPosition.z + offset.z, 0.0f, maxPosition.z)};

      auto kfa = GetPositionAnimation(m_targetPosition.x, m_targetPosition.y);
      m_interactionTracker.TryUpdatePositionWithAnimation(kfa);
    } else {
      m_interactionTracker.TryUpdatePositionBy(offset);
    }
  };

  void TryUpdatePosition(winrt::Windows::Foundation::Numerics::float3 const &position, bool animate) noexcept {
    auto maxPosition = m_interactionTracker.MaxPosition();
    if (animate) {
      auto kfa = GetPositionAnimation(std::min(maxPosition.x, position.x), std::min(maxPosition.y, position.y));
      m_interactionTracker.TryUpdatePositionWithAnimation(kfa);
    } else {
      m_interactionTracker.TryUpdatePosition(
          {std::min(maxPosition.x, position.x),
           std::min(maxPosition.y, position.y),
           std::min(maxPosition.z, position.z)});
    }
  }

  void AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass value) noexcept {
    SetAnimationClass<TTypeRedirects>(value, m_visual);
  }

 private:
  void FireScrollPositionChanged(winrt::Windows::Foundation::Numerics::float2 position) noexcept {
    m_scrollPositionChangedEvent(*this, winrt::make<CompScrollPositionChangedArgs>(position));
  }

  void FireScrollBeginDrag(winrt::Windows::Foundation::Numerics::float2 position) noexcept {
    m_scrollBeginDragEvent(*this, winrt::make<CompScrollPositionChangedArgs>(position));
  }

  void FireScrollEndDrag(winrt::Windows::Foundation::Numerics::float2 position) noexcept {
    m_scrollEndDragEvent(*this, winrt::make<CompScrollPositionChangedArgs>(position));
  }

  void UpdateMaxPosition() noexcept {
    m_interactionTracker.MaxPosition(
        {std::max<float>(m_contentSize.x - m_visualSize.x, 0),
         std::max<float>(m_contentSize.y - m_visualSize.y, 0),
         0});
  }

  bool m_isScrollEnabled{true};
  bool m_horizontal{false};
  bool m_snapToStart{true};
  bool m_inertia{false};
  bool m_custom{false};
  winrt::Windows::Foundation::Numerics::float3 m_targetPosition;
  winrt::Windows::Foundation::Numerics::float3 m_currentPosition;
  winrt::Windows::Foundation::Numerics::float2 m_contentSize{0};
  winrt::Windows::Foundation::Numerics::float2 m_visualSize{0};
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs>>
      m_scrollPositionChangedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs>>
      m_scrollBeginDragEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs>>
      m_scrollEndDragEvent;
  typename TTypeRedirects::SpriteVisual m_visual{nullptr};
  typename TTypeRedirects::SpriteVisual m_contentVisual{nullptr};
  typename TTypeRedirects::InteractionTracker m_interactionTracker{nullptr};
  typename TTypeRedirects::VisualInteractionSource m_visualInteractionSource{nullptr};
};
using WindowsCompScrollerVisual = CompScrollerVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompScrollerVisual = CompScrollerVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompActivityVisual : winrt::implements<
                                CompActivityVisual<TTypeRedirects>,
                                winrt::Microsoft::ReactNative::Composition::Experimental::IActivityVisual,
                                winrt::Microsoft::ReactNative::Composition::Experimental::IVisual,
                                typename TTypeRedirects::IInnerCompositionVisual,
                                IVisualInterop> {
  /************************************************************************************
   * Begin of copy from ProgressRingIndeterminate.cpp which is autogen'd from lottie
   * Changes were made to the namespaces, but otherwise code should be untouched
   ************************************************************************************/

  static constexpr int64_t c_durationTicks{20000000L};
  typename TTypeRedirects::Compositor const _c{nullptr};
  typename TTypeRedirects::ExpressionAnimation const _reusableExpressionAnimation{nullptr};
  typename TTypeRedirects::CompositionPropertySet const _themeProperties{nullptr};
  typename TTypeRedirects::CompositionColorBrush _themeColor_Foreground_0{nullptr};
  typename TTypeRedirects::CompositionColorBrush _themeColor_Foreground_1{nullptr};
  typename TTypeRedirects::ContainerVisual _root{nullptr};
  typename TTypeRedirects::CubicBezierEasingFunction _cubicBezierEasingFunction_0{nullptr};
  typename TTypeRedirects::ExpressionAnimation _rootProgress{nullptr};
  typename TTypeRedirects::StepEasingFunction _holdThenStepEasingFunction{nullptr};

  static void StartProgressBoundAnimation(
      typename TTypeRedirects::CompositionObject target,
      winrt::hstring animatedPropertyName,
      typename TTypeRedirects::CompositionAnimation animation,
      typename TTypeRedirects::ExpressionAnimation controllerProgressExpression) {
    target.StartAnimation(animatedPropertyName, animation);
    const auto controller = target.TryGetAnimationController(animatedPropertyName);
    controller.Pause();
    controller.StartAnimation(L"Progress", controllerProgressExpression);
  }

  void BindProperty(
      typename TTypeRedirects::CompositionObject target,
      winrt::hstring animatedPropertyName,
      winrt::hstring expression,
      winrt::hstring referenceParameterName,
      typename TTypeRedirects::CompositionObject referencedObject) {
    _reusableExpressionAnimation.ClearAllParameters();
    _reusableExpressionAnimation.Expression(expression);
    _reusableExpressionAnimation.SetReferenceParameter(referenceParameterName, referencedObject);
    target.StartAnimation(animatedPropertyName, _reusableExpressionAnimation);
  }

  void BindProperty2(
      typename TTypeRedirects::CompositionObject target,
      winrt::hstring animatedPropertyName,
      winrt::hstring expression,
      winrt::hstring referenceParameterName0,
      typename TTypeRedirects::CompositionObject referencedObject0,
      winrt::hstring referenceParameterName1,
      typename TTypeRedirects::CompositionObject referencedObject1) {
    _reusableExpressionAnimation.ClearAllParameters();
    _reusableExpressionAnimation.Expression(expression);
    _reusableExpressionAnimation.SetReferenceParameter(referenceParameterName0, referencedObject0);
    _reusableExpressionAnimation.SetReferenceParameter(referenceParameterName1, referencedObject1);
    target.StartAnimation(animatedPropertyName, _reusableExpressionAnimation);
  }

  typename TTypeRedirects::ScalarKeyFrameAnimation CreateScalarKeyFrameAnimation(
      float initialProgress,
      float initialValue,
      typename TTypeRedirects::CompositionEasingFunction initialEasingFunction) {
    const auto result = _c.CreateScalarKeyFrameAnimation();
    result.Duration(winrt::Windows::Foundation::TimeSpan{c_durationTicks});
    result.InsertKeyFrame(initialProgress, initialValue, initialEasingFunction);
    return result;
  }

  typename TTypeRedirects::CompositionSpriteShape CreateSpriteShape(
      typename TTypeRedirects::CompositionGeometry geometry,
      winrt::Windows::Foundation::Numerics::float3x2 transformMatrix) {
    const auto result = _c.CreateSpriteShape(geometry);
    result.TransformMatrix(transformMatrix);
    return result;
  }

  // - Layer aggregator
  // Scale:5,5, Offset:<40, 40>
  // Color bound to theme property value: Background
  typename TTypeRedirects::CompositionColorBrush ThemeColor_Background() {
    const auto result = _c.CreateColorBrush();
    BindProperty(
        result,
        L"Color",
        L"ColorRGB(_theme.Background.W*1,_theme.Background.X,_theme.Background.Y,_theme.Background.Z)",
        L"_theme",
        _themeProperties);
    return result;
  }

  // - - Layer aggregator
  // -  Scale:5,5, Offset:<40, 40>
  // ShapeGroup: Ellipse B
  // Color bound to theme property value: Foreground
  typename TTypeRedirects::CompositionColorBrush ThemeColor_Foreground_0() {
    const auto result = _themeColor_Foreground_0 = _c.CreateColorBrush();
    const auto propertySet = result.Properties();
    propertySet.InsertScalar(L"Opacity0", 0.0F);
    BindProperty2(
        result,
        L"Color",
        L"ColorRGB(_theme.Foreground.W*my.Opacity0,_theme.Foreground.X,_theme.Foreground.Y,_theme.Foreground.Z)",
        L"_theme",
        _themeProperties,
        L"my",
        propertySet);
    StartProgressBoundAnimation(propertySet, L"Opacity0", Opacity0ScalarAnimation_0_to_1(), _rootProgress);
    return result;
  }

  // - - Layer aggregator
  // -  Scale:5,5, Offset:<40, 40>
  // ShapeGroup: Ellipse B
  // Color bound to theme property value: Foreground
  typename TTypeRedirects::CompositionColorBrush ThemeColor_Foreground_1() {
    const auto result = _themeColor_Foreground_1 = _c.CreateColorBrush();
    const auto propertySet = result.Properties();
    propertySet.InsertScalar(L"Opacity0", 1.0F);
    BindProperty2(
        result,
        L"Color",
        L"ColorRGB(_theme.Foreground.W*my.Opacity0,_theme.Foreground.X,_theme.Foreground.Y,_theme.Foreground.Z)",
        L"_theme",
        _themeProperties,
        L"my",
        propertySet);
    StartProgressBoundAnimation(propertySet, L"Opacity0", Opacity0ScalarAnimation_1_to_0(), _rootProgress);
    return result;
  }

  // Layer aggregator
  // Transforms for Radial
  typename TTypeRedirects::CompositionContainerShape ContainerShape() {
    const auto result = _c.CreateContainerShape();
    // Offset:<40, 40>, Scale:<5, 5>
    result.TransformMatrix({5.0F, 0.0F, 0.0F, 5.0F, 40.0F, 40.0F});
    const auto shapes = result.Shapes();
    // ShapeGroup: Ellipse B
    shapes.Append(SpriteShape_1());
    // ShapeGroup: Ellipse B
    shapes.Append(SpriteShape_2());
    StartProgressBoundAnimation(
        result, L"RotationAngleInDegrees", RotationAngleInDegreesScalarAnimation_0_to_900(), _rootProgress);
    return result;
  }

  // - Layer aggregator
  // Scale:5,5, Offset:<40, 40>
  // Ellipse Path.EllipseGeometry
  typename TTypeRedirects::CompositionEllipseGeometry Ellipse_7_0() {
    const auto result = _c.CreateEllipseGeometry();
    result.Radius({7.0F, 7.0F});
    return result;
  }

  // - - Layer aggregator
  // -  Scale:5,5, Offset:<40, 40>
  // ShapeGroup: Ellipse B
  // Ellipse Path.EllipseGeometry
  typename TTypeRedirects::CompositionEllipseGeometry Ellipse_7_1() {
    const auto result = _c.CreateEllipseGeometry();
    result.TrimEnd(0.5F);
    result.Radius({7.0F, 7.0F});
    StartProgressBoundAnimation(result, L"TrimStart", TrimStartScalarAnimation_0_to_0p5(), RootProgress());
    return result;
  }

  // - - Layer aggregator
  // -  Scale:5,5, Offset:<40, 40>
  // ShapeGroup: Ellipse B
  // Ellipse Path.EllipseGeometry
  typename TTypeRedirects::CompositionEllipseGeometry Ellipse_7_2() {
    const auto result = _c.CreateEllipseGeometry();
    result.Radius({7.0F, 7.0F});
    StartProgressBoundAnimation(result, L"TrimEnd", TrimEndScalarAnimation_0_to_0p5(), _rootProgress);
    return result;
  }

  // Layer aggregator
  // Ellipse Path
  typename TTypeRedirects::CompositionSpriteShape SpriteShape_0() {
    // Offset:<40, 40>, Scale:<5, 5>
    const auto result = CreateSpriteShape(Ellipse_7_0(), {5.0F, 0.0F, 0.0F, 5.0F, 40.0F, 40.0F});
    result.StrokeBrush(ThemeColor_Background());
    result.StrokeDashCap(TTypeRedirects::CompositionStrokeCap::Round);
    result.StrokeThickness(1.5F);
    return result;
  }

  // - Layer aggregator
  // Scale:5,5, Offset:<40, 40>
  // Ellipse Path
  typename TTypeRedirects::CompositionSpriteShape SpriteShape_1() {
    const auto result = _c.CreateSpriteShape(Ellipse_7_1());
    result.StrokeBrush(ThemeColor_Foreground_0());
    result.StrokeDashCap(TTypeRedirects::CompositionStrokeCap::Round);
    result.StrokeStartCap(TTypeRedirects::CompositionStrokeCap::Round);
    result.StrokeEndCap(TTypeRedirects::CompositionStrokeCap::Round);
    result.StrokeThickness(1.5F);
    return result;
  }

  // - Layer aggregator
  // Scale:5,5, Offset:<40, 40>
  // Ellipse Path
  typename TTypeRedirects::CompositionSpriteShape SpriteShape_2() {
    const auto result = _c.CreateSpriteShape(Ellipse_7_2());
    result.StrokeBrush(ThemeColor_Foreground_1());
    result.StrokeDashCap(TTypeRedirects::CompositionStrokeCap::Round);
    result.StrokeStartCap(TTypeRedirects::CompositionStrokeCap::Round);
    result.StrokeEndCap(TTypeRedirects::CompositionStrokeCap::Round);
    result.StrokeThickness(1.5F);
    return result;
  }

  // The root of the composition.
  typename TTypeRedirects::ContainerVisual Root() {
    const auto result = _root = _c.CreateContainerVisual();
    const auto propertySet = result.Properties();
    propertySet.InsertScalar(L"Progress", 0.0F);
    // Layer aggregator
    result.Children().InsertAtTop(ShapeVisual_0());
    return result;
  }

  typename TTypeRedirects::CubicBezierEasingFunction CubicBezierEasingFunction_0() {
    return _cubicBezierEasingFunction_0 =
               _c.CreateCubicBezierEasingFunction({0.166999996F, 0.166999996F}, {0.833000004F, 0.833000004F});
  }

  typename TTypeRedirects::ExpressionAnimation RootProgress() {
    const auto result = _rootProgress = _c.CreateExpressionAnimation(L"_.Progress");
    result.SetReferenceParameter(L"_", _root);
    return result;
  }

  // Opacity0
  typename TTypeRedirects::ScalarKeyFrameAnimation Opacity0ScalarAnimation_0_to_1() {
    const auto result = CreateScalarKeyFrameAnimation(0.0F, 0.0F, _holdThenStepEasingFunction);
    result.InsertKeyFrame(0.5F, 1.0F, _holdThenStepEasingFunction);
    return result;
  }

  // Opacity0
  typename TTypeRedirects::ScalarKeyFrameAnimation Opacity0ScalarAnimation_1_to_0() {
    const auto result = CreateScalarKeyFrameAnimation(0.0F, 1.0F, _holdThenStepEasingFunction);
    result.InsertKeyFrame(0.5F, 0.0F, _holdThenStepEasingFunction);
    return result;
  }

  // - Layer aggregator
  // Scale:5,5, Offset:<40, 40>
  // Rotation
  typename TTypeRedirects::ScalarKeyFrameAnimation RotationAngleInDegreesScalarAnimation_0_to_900() {
    const auto result = CreateScalarKeyFrameAnimation(0.0F, 0.0F, _holdThenStepEasingFunction);
    result.InsertKeyFrame(0.5F, 450.0F, _cubicBezierEasingFunction_0);
    result.InsertKeyFrame(1.0F, 900.0F, _cubicBezierEasingFunction_0);
    return result;
  }

  // - - - Layer aggregator
  // - -  Scale:5,5, Offset:<40, 40>
  // - ShapeGroup: Ellipse B
  // Ellipse Path.EllipseGeometry
  // TrimEnd
  typename TTypeRedirects::ScalarKeyFrameAnimation TrimEndScalarAnimation_0_to_0p5() {
    const auto result = CreateScalarKeyFrameAnimation(0.0F, 9.99999975E-05F, _holdThenStepEasingFunction);
    result.InsertKeyFrame(0.5F, 0.5F, _cubicBezierEasingFunction_0);
    return result;
  }

  // - - - Layer aggregator
  // - -  Scale:5,5, Offset:<40, 40>
  // - ShapeGroup: Ellipse B
  // Ellipse Path.EllipseGeometry
  // TrimStart
  typename TTypeRedirects::ScalarKeyFrameAnimation TrimStartScalarAnimation_0_to_0p5() {
    const auto result = CreateScalarKeyFrameAnimation(0.0F, 0.0F, StepThenHoldEasingFunction());
    result.InsertKeyFrame(0.5F, 0.0F, HoldThenStepEasingFunction());
    result.InsertKeyFrame(1.0F, 0.5F, CubicBezierEasingFunction_0());
    return result;
  }

  // Layer aggregator
  typename TTypeRedirects::ShapeVisual ShapeVisual_0() {
    const auto result = _c.CreateShapeVisual();
    result.Size({80.0F, 80.0F});
    const auto shapes = result.Shapes();
    // Scale:5,5, Offset:<40, 40>
    shapes.Append(SpriteShape_0());
    // Scale:5,5, Offset:<40, 40>
    shapes.Append(ContainerShape());
    return result;
  }

  typename TTypeRedirects::StepEasingFunction HoldThenStepEasingFunction() {
    const auto result = _holdThenStepEasingFunction = _c.CreateStepEasingFunction();
    result.IsFinalStepSingleFrame(true);
    return result;
  }

  // - - - - Layer aggregator
  // - - -  Scale:5,5, Offset:<40, 40>
  // - - ShapeGroup: Ellipse B
  // - Ellipse Path.EllipseGeometry
  // TrimStart
  typename TTypeRedirects::StepEasingFunction StepThenHoldEasingFunction() {
    const auto result = _c.CreateStepEasingFunction();
    result.IsInitialStepSingleFrame(true);
    return result;
  }

  /************************************************************************************
   * End of copy from ProgressRingIndeterminate.cpp which is autogen'd from lottie
   ************************************************************************************/

  winrt::Windows::Foundation::Numerics::float4 ColorAsVector4(winrt::Windows::UI::Color color) {
    return {
        static_cast<float>(color.R),
        static_cast<float>(color.G),
        static_cast<float>(color.B),
        static_cast<float>(color.A)};
  }

  CompActivityVisual(typename TTypeRedirects::SpriteVisual const &visual)
      : m_visual(visual),
        _c(visual.Compositor()),
        _reusableExpressionAnimation(_c.CreateExpressionAnimation()),
        _themeProperties(_c.CreatePropertySet()) {
    // Default colors
    _themeProperties.InsertVector4(L"Background", ColorAsVector4({0, 0, 0, 0}));
    _themeProperties.InsertVector4(L"Foreground", ColorAsVector4({0, 0, 0, 0}));

    visual.Children().InsertAtTop(Root());
    StartAnimation();
  }

  void Brush(winrt::Microsoft::ReactNative::Composition::Experimental::IBrush brush) noexcept {
    auto innerBrush = TTypeRedirects::CompositionContextHelper::InnerBrush(brush);
    if (brush) {
      _themeProperties.InsertVector4(
          L"Foreground", ColorAsVector4(innerBrush.as<TTypeRedirects::CompositionColorBrush>().Color()));
    } else {
      _themeProperties.InsertVector4(L"Foreground", ColorAsVector4({0, 0, 0, 0}));
    }
  }

  void Size(float radius) noexcept {
    auto scale = radius / 40.0f;
    _root.Scale({scale, scale, 1.0f});
  }

  typename TTypeRedirects::Visual InnerVisual() const noexcept {
    return m_visual;
  }

  void InsertAt(
      const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual,
      uint32_t index) noexcept {
    auto containerChildren = m_contentVisual.Children();
    auto compVisual = typename TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    if (index == 0) {
      containerChildren.InsertAtBottom(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
    auto compVisual = typename TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_contentVisual.Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<typename TTypeRedirects::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return typename TTypeRedirects::CompositionContextHelper::CreateVisual(it.Current());
  }

  void Opacity(float opacity) noexcept {
    m_visual.Opacity(opacity);
  }

  void Scale(winrt::Windows::Foundation::Numerics::float3 const &scale) noexcept {
    m_visual.Scale(scale);
  }

  void TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4 const &transform) noexcept {
    m_visual.TransformMatrix(transform);
  }

  void RotationAngle(float rotation) noexcept {
    m_visual.RotationAngle(rotation);
  }

  void IsVisible(bool isVisible) noexcept {
    m_visual.IsVisible(isVisible);
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept {
    m_visual.Size(size);
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept {
    m_visual.Offset(offset);
  }

  void Offset(
      winrt::Windows::Foundation::Numerics::float3 offset,
      winrt::Windows::Foundation::Numerics::float3 relativeAdjustment) noexcept {
    m_visual.Offset(offset);
    m_visual.RelativeOffsetAdjustment(relativeAdjustment);
  }

  void RelativeSizeWithOffset(
      winrt::Windows::Foundation::Numerics::float2 size,
      winrt::Windows::Foundation::Numerics::float2 relativeSizeAdjustment) noexcept {
    m_visual.Size(size);
    m_visual.RelativeSizeAdjustment(relativeSizeAdjustment);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility BackfaceVisibility() const noexcept {
    return static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility>(
        m_visual.BackfaceVisibility());
  }

  void BackfaceVisibility(winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility value) noexcept {
    m_visual.BackfaceVisibility(static_cast<typename TTypeRedirects::CompositionBackfaceVisibility>(value));
  }

  winrt::hstring Comment() const noexcept {
    return m_visual.Comment();
  }

  void Comment(winrt::hstring value) noexcept {
    m_visual.Comment(value);
  }

  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept {
    if (!clippingPath) {
      m_visual.Clip(nullptr);
      return;
    }
    auto geometry = winrt::make<GeometrySource>(clippingPath);
    auto path = typename TTypeRedirects::CompositionPath(geometry);
    auto pathgeo = m_visual.Compositor().CreatePathGeometry(path);
    auto clip = m_visual.Compositor().CreateGeometricClip(pathgeo);
    m_visual.Clip(clip);
  }

  void AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass value) noexcept {
    SetAnimationClass<TTypeRedirects>(value, m_visual);
  }

  void StartAnimation() noexcept {
    auto easing = _c.CreateLinearEasingFunction();
    const auto progressAnimation = _c.CreateScalarKeyFrameAnimation();
    progressAnimation.Duration(winrt::Windows::Foundation::TimeSpan{c_durationTicks});
    progressAnimation.InsertKeyFrame(0.0f, 0.0f);
    progressAnimation.InsertKeyFrame(1.0f, 1.0f, easing);
    progressAnimation.IterationBehavior(TTypeRedirects::AnimationIterationBehavior::Forever);

    _root.Properties().StartAnimation(L"Progress", progressAnimation);
    _root.StartAnimation(L"Progress", progressAnimation);
  }

  void StopAnimation() noexcept {
    _root.Properties().InsertScalar(L"Progress", 0.7f);
    _root.Properties().StopAnimation(L"Progress");
    _root.StopAnimation(L"Progress");
  }

 private:
  typename TTypeRedirects::SpriteVisual m_visual{nullptr};
  typename TTypeRedirects::SpriteVisual m_contentVisual{nullptr};
};
using WindowsCompActivityVisual = CompActivityVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompActivityVisual = CompActivityVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompCaretVisual : winrt::implements<
                             CompCaretVisual<TTypeRedirects>,
                             winrt::Microsoft::ReactNative::Composition::Experimental::ICaretVisual> {
  CompCaretVisual(typename TTypeRedirects::Compositor const &compositor)
      : m_compositor(compositor),
        m_compVisual(compositor.CreateSpriteVisual()),
        m_opacityAnimation(compositor.CreateScalarKeyFrameAnimation()) {
    m_visual = CreateVisual();

    // TODO should make the caret use an invert brush by default
    m_compVisual.Brush(m_compositor.CreateColorBrush({255, 0, 0, 0} /* Black */));
    m_compVisual.Opacity(1.0f);

    // Blinking animation
    constexpr float ftCaretFadePct = 0.2385714285714f;
    constexpr float stayVisFrame = (1.0f - ftCaretFadePct) / 2.0f;
    constexpr float fadeVisFrame = ftCaretFadePct / 2.0f;

    m_opacityAnimation.InsertKeyFrame(0.0f, 1.0f);
    m_opacityAnimation.InsertKeyFrame(stayVisFrame, 1.0f);
    m_opacityAnimation.InsertKeyFrame(stayVisFrame + fadeVisFrame, 0.0f, m_compositor.CreateLinearEasingFunction());
    m_opacityAnimation.InsertKeyFrame(stayVisFrame + fadeVisFrame + stayVisFrame, 0.0f);
    m_opacityAnimation.InsertKeyFrame(1.0f, 1.0f, m_compositor.CreateLinearEasingFunction());
    m_opacityAnimation.Duration(std::chrono::milliseconds{1000});
    m_opacityAnimation.IterationBehavior(TTypeRedirects::AnimationIterationBehavior::Count);
    m_opacityAnimation.IterationCount(500);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual CreateVisual() const noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual InnerVisual() const noexcept {
    return m_visual;
  }

  void Brush(winrt::Microsoft::ReactNative::Composition::Experimental::IBrush brush) noexcept {
    m_compVisual.Brush(TTypeRedirects::CompositionContextHelper::InnerBrush(brush));
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 size) noexcept {
    m_compVisual.Size(size);
  }

  void Position(winrt::Windows::Foundation::Numerics::float2 position) noexcept {
    m_compVisual.Offset({position.x, position.y, 0.0f});
  }

  bool IsVisible() const noexcept {
    return m_isVisible;
  }

  void IsVisible(bool value) noexcept {
    if (m_isVisible == value)
      return;
    m_isVisible = value;
    if (value) {
      m_compVisual.StartAnimation(L"opacity", m_opacityAnimation);
    } else {
      m_compVisual.StopAnimation(L"opacity");
      m_compVisual.Opacity(0.0f);
    }
  }

  void AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass value) noexcept {
    SetAnimationClass(value, m_compVisual);
  }

 private:
  bool m_isVisible{true};
  typename TTypeRedirects::SpriteVisual m_compVisual;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_visual;
  typename TTypeRedirects::ScalarKeyFrameAnimation m_opacityAnimation;
  typename TTypeRedirects::Compositor m_compositor{nullptr};
};

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual CompCaretVisual<WindowsTypeRedirects>::CreateVisual()
    const noexcept {
  return winrt::make<Composition::Experimental::WindowsCompSpriteVisual>(m_compVisual);
}
using WindowsCompCaretVisual = CompCaretVisual<WindowsTypeRedirects>;

#ifdef USE_WINUI3
winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
CompCaretVisual<MicrosoftTypeRedirects>::CreateVisual() const noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompSpriteVisual>(m_compVisual);
}
using MicrosoftCompCaretVisual = CompCaretVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompFocusVisual : winrt::implements<
                             CompFocusVisual<TTypeRedirects>,
                             winrt::Microsoft::ReactNative::Composition::Experimental::IFocusVisual> {
  CompFocusVisual(typename TTypeRedirects::Compositor const &compositor)
      : m_compVisual(compositor.CreateSpriteVisual()), m_brush(compositor.CreateNineGridBrush()) {
    m_visual = CreateVisual();

    m_compVisual.Opacity(1.0f);
    m_compVisual.RelativeSizeAdjustment({1, 1});

    m_brush.Source(compositor.CreateColorBrush({255, 0, 0, 0} /* Black */));
    m_brush.IsCenterHollow(true);
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual CreateVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual InnerVisual() const noexcept {
    return m_visual;
  }

  bool IsFocused() const noexcept {
    return m_compVisual.Brush() != nullptr;
  }

  void IsFocused(bool value) noexcept {
    if (value) {
      m_compVisual.Brush(m_brush);
    } else {
      m_compVisual.Brush(nullptr);
    }
  }

  float ScaleFactor() const noexcept {
    return m_scaleFactor;
  }

  void ScaleFactor(float scaleFactor) noexcept {
    if (m_scaleFactor == scaleFactor) {
      return;
    }
    m_scaleFactor = scaleFactor;
    auto inset = 2 * scaleFactor;
    m_brush.SetInsets(inset, inset, inset, inset);
  }

  void AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass value) noexcept {
    SetAnimationClass(value, m_compVisual);
  }

 private:
  float m_scaleFactor{0};
  typename TTypeRedirects::CompositionNineGridBrush m_brush;
  typename TTypeRedirects::SpriteVisual m_compVisual;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_visual{nullptr};
};

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
CompFocusVisual<WindowsTypeRedirects>::CreateVisual() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompSpriteVisual>(m_compVisual);
}
using WindowsCompFocusVisual = CompFocusVisual<WindowsTypeRedirects>;

#ifdef USE_WINUI3
winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
CompFocusVisual<MicrosoftTypeRedirects>::CreateVisual() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompSpriteVisual>(m_compVisual);
}
using MicrosoftCompFocusVisual = CompFocusVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompContext : winrt::implements<
                         CompContext<TTypeRedirects>,
                         winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext,
                         typename TTypeRedirects::IInnerCompositionCompositor,
                         ICompositionContextInterop> {
  CompContext(typename TTypeRedirects::Compositor const &compositor) : m_compositor(compositor) {}

  winrt::com_ptr<ID2D1Factory1> D2DFactory() noexcept {
    if (!m_d2dFactory) {
      // Initialize Direct2D resources.
      // #if defined(_DEBUG)
      //     D2D1_FACTORY_OPTIONS d2d1FactoryOptions{D2D1_DEBUG_LEVEL_INFORMATION};
      // #else
      D2D1_FACTORY_OPTIONS d2d1FactoryOptions{D2D1_DEBUG_LEVEL_NONE};
      // #endif

      D2D1CreateFactory(
          D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &d2d1FactoryOptions, m_d2dFactory.put_void());
    }
    return m_d2dFactory;
  }

  void D2DFactory(ID2D1Factory1 **outD2DFactory) noexcept {
    D2DFactory().copy_to(outD2DFactory);
  }

  winrt::com_ptr<ID3D11Device> D3DDevice() noexcept {
    // This flag adds support for surfaces with a different color channel ordering than the API default.
    // You need it for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    // #if defined(_DEBUG)
    //   creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    // #endif

    // This array defines the set of DirectX hardware feature levels this app  supports.
    // The ordering is important and you should  preserve it.
    // Don't forget to declare your app's minimum required feature level in its
    // description.  All apps are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1};
    if (!m_d3dDevice) {
      D3D11CreateDevice(
          nullptr, // specify null to use the default adapter
          D3D_DRIVER_TYPE_HARDWARE,
          0,
          creationFlags, // optionally set debug and Direct2D compatibility flags
          featureLevels, // list of feature levels this app can support
          ARRAYSIZE(featureLevels), // number of possible feature levels
          D3D11_SDK_VERSION,
          m_d3dDevice.put(), // returns the Direct3D device created
          nullptr /*&m_featureLevel*/, // returns feature level of device created
          nullptr /*&context*/ // returns the device immediate context
      );
    }
    return m_d3dDevice;
  }

  winrt::com_ptr<ID2D1Device> D2DDevice() noexcept {
    if (!m_d2dDevice) {
      winrt::com_ptr<IDXGIDevice> dxgiDevice;
      // Obtain the underlying DXGI device of the Direct3D11 device.
      D3DDevice().as(dxgiDevice);

      // Obtain the Direct2D device for 2-D rendering.
      winrt::check_hresult(D2DFactory()->CreateDevice(dxgiDevice.get(), m_d2dDevice.put()));
    }
    return m_d2dDevice;
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual CreateSpriteVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual
  CreateRoundedRectangleVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IScrollVisual CreateScrollerVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IActivityVisual CreateActivityVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IDropShadow CreateDropShadow() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush CreateColorBrush(
      winrt::Windows::UI::Color color) noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush CreateDrawingSurfaceBrush(
      winrt::Windows::Foundation::Size surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode) noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::ICaretVisual CreateCaretVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IFocusVisual CreateFocusVisual() noexcept;

  typename TTypeRedirects::CompositionGraphicsDevice CompositionGraphicsDevice() noexcept;

  typename TTypeRedirects::Compositor InnerCompositor() const noexcept {
    return m_compositor;
  }

 private:
  typename TTypeRedirects::Compositor m_compositor{nullptr};
  winrt::com_ptr<ID2D1Factory1> m_d2dFactory;
  winrt::com_ptr<ID3D11Device> m_d3dDevice;
  winrt::com_ptr<ID2D1Device> m_d2dDevice;
  typename TTypeRedirects::CompositionGraphicsDevice m_compositionGraphicsDevice{nullptr};
  winrt::com_ptr<ID3D11DeviceContext> m_d3dDeviceContext;
};

winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual
CompContext<WindowsTypeRedirects>::CreateSpriteVisual() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompSpriteVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IScrollVisual
CompContext<WindowsTypeRedirects>::CreateScrollerVisual() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompScrollerVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual
CompContext<WindowsTypeRedirects>::CreateRoundedRectangleVisual() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompRoundedRectangleVisual>(m_compositor.CreateShapeVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IActivityVisual
CompContext<WindowsTypeRedirects>::CreateActivityVisual() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompActivityVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IDropShadow
CompContext<WindowsTypeRedirects>::CreateDropShadow() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompDropShadow>(m_compositor.CreateDropShadow());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush CompContext<WindowsTypeRedirects>::CreateColorBrush(
    winrt::Windows::UI::Color color) noexcept {
  return winrt::make<Composition::Experimental::WindowsCompBrush>(m_compositor.CreateColorBrush(color));
}

winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush
CompContext<WindowsTypeRedirects>::CreateDrawingSurfaceBrush(
    winrt::Windows::Foundation::Size surfaceSize,
    winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
    winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode) noexcept {
  return winrt::make<Composition::Experimental::WindowsCompDrawingSurfaceBrush>(
      m_compositor, CompositionGraphicsDevice().CreateDrawingSurface(surfaceSize, pixelFormat, alphaMode));
}

winrt::Microsoft::ReactNative::Composition::Experimental::ICaretVisual
CompContext<WindowsTypeRedirects>::CreateCaretVisual() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompCaretVisual>(m_compositor);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IFocusVisual
CompContext<WindowsTypeRedirects>::CreateFocusVisual() noexcept {
  return winrt::make<Composition::Experimental::WindowsCompFocusVisual>(m_compositor);
}

template <>
winrt::Windows::UI::Composition::CompositionGraphicsDevice
CompContext<WindowsTypeRedirects>::CompositionGraphicsDevice() noexcept {
  if (!m_compositionGraphicsDevice) {
    // To create a composition graphics device, we need to QI for another interface

    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositorInterop> compositorInterop{
        m_compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>()};

    // Create a graphics device backed by our D3D device
    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionGraphicsDevice> compositionGraphicsDeviceIface;
    winrt::check_hresult(
        compositorInterop->CreateGraphicsDevice(D2DDevice().get(), compositionGraphicsDeviceIface.put()));

    compositionGraphicsDeviceIface.as(m_compositionGraphicsDevice);
  }
  return m_compositionGraphicsDevice;
}

using WindowsCompContext = CompContext<WindowsTypeRedirects>;

#ifdef USE_WINUI3
winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual
CompContext<MicrosoftTypeRedirects>::CreateSpriteVisual() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompSpriteVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IScrollVisual
CompContext<MicrosoftTypeRedirects>::CreateScrollerVisual() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompScrollerVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual
CompContext<MicrosoftTypeRedirects>::CreateRoundedRectangleVisual() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompRoundedRectangleVisual>(m_compositor.CreateShapeVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IActivityVisual
CompContext<MicrosoftTypeRedirects>::CreateActivityVisual() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompActivityVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IDropShadow
CompContext<MicrosoftTypeRedirects>::CreateDropShadow() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompDropShadow>(m_compositor.CreateDropShadow());
}

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush CompContext<MicrosoftTypeRedirects>::CreateColorBrush(
    winrt::Windows::UI::Color color) noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompBrush>(m_compositor.CreateColorBrush(color));
}

winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush
CompContext<MicrosoftTypeRedirects>::CreateDrawingSurfaceBrush(
    winrt::Windows::Foundation::Size surfaceSize,
    winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
    winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode) noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompDrawingSurfaceBrush>(
      m_compositor,
      CompositionGraphicsDevice().CreateDrawingSurface(
          surfaceSize,
          static_cast<winrt::Microsoft::Graphics::DirectX::DirectXPixelFormat>(pixelFormat),
          static_cast<winrt::Microsoft::Graphics::DirectX::DirectXAlphaMode>(alphaMode)));
}

winrt::Microsoft::ReactNative::Composition::Experimental::ICaretVisual
CompContext<MicrosoftTypeRedirects>::CreateCaretVisual() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompCaretVisual>(m_compositor);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IFocusVisual
CompContext<MicrosoftTypeRedirects>::CreateFocusVisual() noexcept {
  return winrt::make<Composition::Experimental::MicrosoftCompFocusVisual>(m_compositor);
}

template <>
winrt::Microsoft::UI::Composition::CompositionGraphicsDevice
CompContext<MicrosoftTypeRedirects>::CompositionGraphicsDevice() noexcept {
  if (!m_compositionGraphicsDevice) {
    winrt::check_hresult(m_compositor.as<winrt::Microsoft::UI::Composition::ICompositorInterop>()->CreateGraphicsDevice(
        D2DDevice().get(), &m_compositionGraphicsDevice));
  }
  return m_compositionGraphicsDevice;
}

using MicrosoftCompContext = CompContext<MicrosoftTypeRedirects>;
#endif

} // namespace Microsoft::ReactNative::Composition::Experimental

namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation {

ICompositionContext SystemCompositionContextHelper::CreateContext(
    winrt::Windows::UI::Composition::Compositor const &compositor) noexcept {
  return winrt::make<::Microsoft::ReactNative::Composition::Experimental::WindowsCompContext>(compositor);
}

IVisual SystemCompositionContextHelper::CreateVisual(winrt::Windows::UI::Composition::Visual const &visual) noexcept {
  if (auto spriteVisual = visual.try_as<winrt::Windows::UI::Composition::SpriteVisual>())
    return winrt::make<::Microsoft::ReactNative::Composition::Experimental::WindowsCompSpriteVisual>(spriteVisual);
  return winrt::make<::Microsoft::ReactNative::Composition::Experimental::WindowsCompVisual>(visual);
}

winrt::Windows::UI::Composition::Compositor SystemCompositionContextHelper::InnerCompositor(
    ICompositionContext context) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionCompositor> s;
  context.try_as(s);
  return s ? s->InnerCompositor() : nullptr;
}

winrt::Windows::UI::Composition::Visual SystemCompositionContextHelper::InnerVisual(IVisual visual) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionVisual> s;
  visual.try_as(s);
  return s ? s->InnerVisual() : nullptr;
}

winrt::Windows::UI::Composition::DropShadow SystemCompositionContextHelper::InnerDropShadow(
    IDropShadow shadow) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionDropShadow> s;
  shadow.try_as(s);
  return s ? s->InnerShadow() : nullptr;
}

winrt::Windows::UI::Composition::CompositionBrush SystemCompositionContextHelper::InnerBrush(IBrush brush) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionBrush> s;
  brush.try_as(s);
  return s ? s->InnerBrush() : nullptr;
}

winrt::Windows::UI::Composition::ICompositionSurface SystemCompositionContextHelper::InnerSurface(
    IDrawingSurfaceBrush surface) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionDrawingSurfaceInner> s;
  surface.try_as(s);
  return s ? s->Inner() : nullptr;
}

#ifdef USE_WINUI3
ICompositionContext MicrosoftCompositionContextHelper::CreateContext(
    winrt::Microsoft::UI::Composition::Compositor const &compositor) noexcept {
  return winrt::make<::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompContext>(compositor);
}

IVisual MicrosoftCompositionContextHelper::CreateVisual(
    winrt::Microsoft::UI::Composition::Visual const &visual) noexcept {
  if (auto spriteVisual = visual.try_as<winrt::Microsoft::UI::Composition::SpriteVisual>())
    return winrt::make<::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompSpriteVisual>(spriteVisual);
  return winrt::make<::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompVisual>(visual);
}

winrt::Microsoft::UI::Composition::Compositor MicrosoftCompositionContextHelper::InnerCompositor(
    ICompositionContext context) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IMicrosoftCompositionCompositor> s;
  context.try_as(s);
  return s ? s->InnerCompositor() : nullptr;
}

winrt::Microsoft::UI::Composition::Visual MicrosoftCompositionContextHelper::InnerVisual(IVisual visual) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IMicrosoftCompositionVisual> s;
  visual.try_as(s);
  return s ? s->InnerVisual() : nullptr;
}

winrt::Microsoft::UI::Composition::DropShadow MicrosoftCompositionContextHelper::InnerDropShadow(
    IDropShadow shadow) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IMicrosoftCompositionDropShadow> s;
  shadow.try_as(s);
  return s ? s->InnerShadow() : nullptr;
}

winrt::Microsoft::UI::Composition::CompositionBrush MicrosoftCompositionContextHelper::InnerBrush(
    IBrush brush) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IMicrosoftCompositionBrush> s;
  brush.try_as(s);
  return s ? s->InnerBrush() : nullptr;
}

winrt::Microsoft::UI::Composition::ICompositionSurface MicrosoftCompositionContextHelper::InnerSurface(
    IDrawingSurfaceBrush surface) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IMicrosoftCompositionDrawingSurfaceInner> s;
  surface.try_as(s);
  return s ? s->Inner() : nullptr;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush MicrosoftCompositionContextHelper::WrapBrush(
    const winrt::Microsoft::UI::Composition::CompositionBrush &brush) noexcept {
  return winrt::make<::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompBrush>(brush);
}

#endif

} // namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation
