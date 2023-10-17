
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.WindowsCompositionContextHelper.g.cpp")
#include "Composition.WindowsCompositionContextHelper.g.cpp"
#endif
#ifdef USE_WINUI3
#if __has_include("Composition.MicrosoftCompositionContextHelper.g.cpp")
#include "Composition.MicrosoftCompositionContextHelper.g.cpp"
#endif
#endif

#include <Windows.Graphics.Interop.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.interactions.h>
#include "CompositionHelpers.h"

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Composition.interactions.h>
#include <winrt/Microsoft.UI.Composition.interop.h>
#endif

namespace Microsoft::ReactNative::Composition {

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
  using CompositionDrawingSurface = winrt::Windows::UI::Composition::CompositionDrawingSurface;
  using CompositionEllipseGeometry = winrt::Windows::UI::Composition::CompositionEllipseGeometry;
  using CompositionNineGridBrush = winrt::Windows::UI::Composition::CompositionNineGridBrush;
  using CompositionPath = winrt::Windows::UI::Composition::CompositionPath;
  using CompositionSpriteShape = winrt::Windows::UI::Composition::CompositionSpriteShape;
  using CompositionStretch = winrt::Windows::UI::Composition::CompositionStretch;
  using CompositionSurfaceBrush = winrt::Windows::UI::Composition::CompositionSurfaceBrush;
  using Compositor = winrt::Windows::UI::Composition::Compositor;
  using ContainerVisual = winrt::Windows::UI::Composition::ContainerVisual;
  using DropShadow = winrt::Windows::UI::Composition::DropShadow;
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
  using CompositionContextHelper = winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper;
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
  using CompositionDrawingSurface = winrt::Microsoft::UI::Composition::CompositionDrawingSurface;
  using CompositionEllipseGeometry = winrt::Microsoft::UI::Composition::CompositionEllipseGeometry;
  using CompositionNineGridBrush = winrt::Microsoft::UI::Composition::CompositionNineGridBrush;
  using CompositionPath = winrt::Microsoft::UI::Composition::CompositionPath;
  using CompositionSpriteShape = winrt::Microsoft::UI::Composition::CompositionSpriteShape;
  using CompositionStretch = winrt::Microsoft::UI::Composition::CompositionStretch;
  using CompositionSurfaceBrush = winrt::Microsoft::UI::Composition::CompositionSurfaceBrush;
  using Compositor = winrt::Microsoft::UI::Composition::Compositor;
  using ContainerVisual = winrt::Microsoft::UI::Composition::ContainerVisual;
  using DropShadow = winrt::Microsoft::UI::Composition::DropShadow;
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
  using CompositionContextHelper = winrt::Microsoft::ReactNative::Composition::MicrosoftCompositionContextHelper;
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
                            winrt::Microsoft::ReactNative::Composition::IDropShadow,
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
                       winrt::Microsoft::ReactNative::Composition::IBrush,
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
                                     winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush,
                                     winrt::Microsoft::ReactNative::Composition::IBrush,
                                     typename TTypeRedirects::IInnerCompositionBrush,
                                     ICompositionDrawingSurfaceInterop,
                                     typename TTypeRedirects::IInnerCompositionDrawingSurface> {
  CompDrawingSurfaceBrush(
      const typename TTypeRedirects::Compositor &compositor,
      typename TTypeRedirects::CompositionDrawingSurface const &drawingSurface)
      : m_brush(compositor.CreateSurfaceBrush(drawingSurface)) {
    drawingSurface.as(m_drawingSurfaceInterop);
  }

  HRESULT BeginDraw(ID2D1DeviceContext **deviceContextOut, POINT *offset) noexcept {
    return m_drawingSurfaceInterop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), (void **)deviceContextOut, offset);
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

  void Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch mode) noexcept {
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::CompositionStretch>(
            TTypeRedirects::CompositionStretch::None) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::CompositionStretch>(
            TTypeRedirects::CompositionStretch::Fill) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::Fill);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::CompositionStretch>(
            TTypeRedirects::CompositionStretch::Uniform) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::Uniform);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::CompositionStretch>(
            TTypeRedirects::CompositionStretch::UniformToFill) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::UniformToFill);

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
struct CompVisual : public winrt::implements<
                        CompVisual<TTypeRedirects>,
                        winrt::Microsoft::ReactNative::Composition::IVisual,
                        typename TTypeRedirects::IInnerCompositionVisual,
                        IVisualInterop> {
  CompVisual(typename TTypeRedirects::Visual const &visual) : m_visual(visual) {}

  typename TTypeRedirects::Visual InnerVisual() const noexcept override {
    return m_visual;
  }

  void InsertAt(const winrt::Microsoft::ReactNative::Composition::IVisual &visual, uint32_t index) noexcept {
    auto containerChildren = m_visual.as<typename TTypeRedirects::ContainerVisual>().Children();
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    if (index == 0 || containerChildren.Count() == 0) {
      containerChildren.InsertAtTop(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(const winrt::Microsoft::ReactNative::Composition::IVisual &visual) noexcept {
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_visual.as<typename TTypeRedirects::ContainerVisual>().Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual GetAt(uint32_t index) noexcept {
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

  winrt::Microsoft::ReactNative::Composition::BackfaceVisibility BackfaceVisibility() {
    return static_cast<winrt::Microsoft::ReactNative::Composition::BackfaceVisibility>(m_visual.BackfaceVisibility());
  }

  void BackfaceVisibility(winrt::Microsoft::ReactNative::Composition::BackfaceVisibility value) {
    m_visual.BackfaceVisibility(static_cast<typename TTypeRedirects::CompositionBackfaceVisibility>(value));
  }

 private:
  typename TTypeRedirects::Visual m_visual;
};
using WindowsCompVisual = CompVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompVisual = CompVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompSpriteVisual : winrt::implements<
                              CompSpriteVisual<TTypeRedirects>,
                              winrt::Microsoft::ReactNative::Composition::ISpriteVisual,
                              winrt::Microsoft::ReactNative::Composition::IVisual,
                              typename TTypeRedirects::IInnerCompositionVisual,
                              IVisualInterop> {
  CompSpriteVisual(typename TTypeRedirects::SpriteVisual const &visual) : m_visual(visual) {}

  typename TTypeRedirects::Visual InnerVisual() const noexcept override {
    return m_visual;
  }

  void InsertAt(const winrt::Microsoft::ReactNative::Composition::IVisual &visual, uint32_t index) noexcept {
    auto containerChildren = m_visual.Children();
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

  void Remove(const winrt::Microsoft::ReactNative::Composition::IVisual &visual) noexcept {
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_visual.Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<typename TTypeRedirects::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return TTypeRedirects::CompositionContextHelper::CreateVisual(it.Current());
  }

  void Brush(const winrt::Microsoft::ReactNative::Composition::IBrush &brush) noexcept {
    m_visual.Brush(TTypeRedirects::CompositionContextHelper::InnerBrush(brush));
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

  winrt::Microsoft::ReactNative::Composition::BackfaceVisibility BackfaceVisibility() {
    return static_cast<winrt::Microsoft::ReactNative::Composition::BackfaceVisibility>(m_visual.BackfaceVisibility());
  }

  void BackfaceVisibility(winrt::Microsoft::ReactNative::Composition::BackfaceVisibility value) {
    m_visual.BackfaceVisibility(static_cast<typename TTypeRedirects::CompositionBackfaceVisibility>(value));
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

  void Shadow(const winrt::Microsoft::ReactNative::Composition::IDropShadow &shadow) noexcept {
    m_visual.Shadow(TTypeRedirects::CompositionContextHelper::InnerDropShadow(shadow));
  }

 private:
  typename TTypeRedirects::SpriteVisual m_visual;
};
using WindowsCompSpriteVisual = CompSpriteVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompSpriteVisual = CompSpriteVisual<MicrosoftTypeRedirects>;
#endif

struct CompScrollPositionChangedArgs : winrt::implements<
                                           CompScrollPositionChangedArgs,
                                           winrt::Microsoft::ReactNative::Composition::IScrollPositionChangedArgs> {
  CompScrollPositionChangedArgs(winrt::Windows::Foundation::Numerics::float2 position) : m_position(position) {}

  winrt::Windows::Foundation::Numerics::float2 Position() {
    return m_position;
  }

 private:
  winrt::Windows::Foundation::Numerics::float2 m_position;
};

template <typename TTypeRedirects>
struct CompScrollerVisual : winrt::implements<
                                CompScrollerVisual<TTypeRedirects>,
                                winrt::Microsoft::ReactNative::Composition::IScrollVisual,
                                winrt::Microsoft::ReactNative::Composition::IVisual,
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
    }
    void InteractingStateEntered(
        typename TTypeRedirects::InteractionTracker sender,
        typename TTypeRedirects::InteractionTrackerInteractingStateEnteredArgs args) noexcept {}
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

    m_visualInteractionSource.PositionXSourceMode(TTypeRedirects::InteractionSourceMode::EnabledWithInertia);
    m_visualInteractionSource.PositionYSourceMode(TTypeRedirects::InteractionSourceMode::EnabledWithInertia);
    m_visualInteractionSource.ScaleSourceMode(TTypeRedirects::InteractionSourceMode::Disabled);

    m_visualInteractionSource.ManipulationRedirectionMode(
        TTypeRedirects::VisualInteractionSourceRedirectionMode::CapableTouchpadAndPointerWheel);
    m_interactionTracker.InteractionSources().Add(m_visualInteractionSource);

    auto positionExpression = compositor.CreateExpressionAnimation(L"-tracker.Position");
    positionExpression.SetReferenceParameter(L"tracker", m_interactionTracker);
    m_contentVisual.StartAnimation(L"Offset", positionExpression);
  }

  typename TTypeRedirects::Visual InnerVisual() const noexcept {
    return m_visual;
  }

  void InsertAt(const winrt::Microsoft::ReactNative::Composition::IVisual &visual, uint32_t index) noexcept {
    auto containerChildren = m_contentVisual.Children();
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    if (index == 0 || containerChildren.Count() == 0) {
      containerChildren.InsertAtTop(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(const winrt::Microsoft::ReactNative::Composition::IVisual &visual) noexcept {
    auto compVisual = TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_contentVisual.Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<typename TTypeRedirects::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return TTypeRedirects::CompositionContextHelper::CreateVisual(it.Current());
  }

  void Brush(const winrt::Microsoft::ReactNative::Composition::IBrush &brush) noexcept {
    m_visual.Brush(TTypeRedirects::CompositionContextHelper::InnerBrush(brush));
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

  winrt::Microsoft::ReactNative::Composition::BackfaceVisibility BackfaceVisibility() {
    return static_cast<winrt::Microsoft::ReactNative::Composition::BackfaceVisibility>(m_visual.BackfaceVisibility());
  }

  void BackfaceVisibility(winrt::Microsoft::ReactNative::Composition::BackfaceVisibility value) {
    m_visual.BackfaceVisibility(static_cast<typename TTypeRedirects::CompositionBackfaceVisibility>(value));
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

  void Shadow(const winrt::Microsoft::ReactNative::Composition::IDropShadow &shadow) noexcept {
    m_visual.Shadow(TTypeRedirects::CompositionContextHelper::InnerDropShadow(shadow));
  }

  winrt::event_token ScrollPositionChanged(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::IScrollPositionChangedArgs> const &handler) {
    return m_scrollPositionChangedEvent.add(handler);
  }

  void ScrollPositionChanged(winrt::event_token const &token) noexcept {
    m_scrollPositionChangedEvent.remove(token);
  }

  void ContentSize(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept {
    m_contentSize = size;
    m_contentVisual.Size(size);
    UpdateMaxPosition();
  }

  winrt::Windows::Foundation::Numerics::float3 ScrollPosition() noexcept {
    return m_interactionTracker.Position();
  }

  // ChangeOffsets scrolling constants
  static constexpr int64_t s_offsetsChangeMsPerUnit{5};
  static constexpr int64_t s_offsetsChangeMinMs{50};
  static constexpr int64_t s_offsetsChangeMaxMs{1000};

  typename TTypeRedirects::CompositionAnimation GetPositionAnimation(float x, float y) {
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

 private:
  void FireScrollPositionChanged(winrt::Windows::Foundation::Numerics::float2 position) {
    m_scrollPositionChangedEvent(*this, winrt::make<CompScrollPositionChangedArgs>(position));
  }

  void UpdateMaxPosition() {
    m_interactionTracker.MaxPosition(
        {std::max<float>(m_contentSize.x - m_visualSize.x, 0),
         std::max<float>(m_contentSize.y - m_visualSize.y, 0),
         0});
  }

  bool m_inertia{false};
  bool m_custom{false};
  winrt::Windows::Foundation::Numerics::float3 m_targetPosition;
  winrt::Windows::Foundation::Numerics::float3 m_currentPosition;
  winrt::Windows::Foundation::Numerics::float2 m_contentSize{0};
  winrt::Windows::Foundation::Numerics::float2 m_visualSize{0};
  winrt::event<
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::IScrollPositionChangedArgs>>
      m_scrollPositionChangedEvent;
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
                                winrt::Microsoft::ReactNative::Composition::IActivityVisual,
                                winrt::Microsoft::ReactNative::Composition::IVisual,
                                typename TTypeRedirects::IInnerCompositionVisual,
                                IVisualInterop> {
  CompActivityVisual(typename TTypeRedirects::SpriteVisual const &visual) : m_visual(visual) {
    auto compositor = m_visual.Compositor();
    m_contentVisual = compositor.CreateSpriteVisual();

    // Create loading circles
    for (int i = 0; i < 4; i++) {
      auto loadingCircle = createLoadingCircle(compositor, i * 200);
      m_visual.Children().InsertAtTop(loadingCircle);
    }
  }

  void Color(winrt::Windows::UI::Color color) noexcept {
    // Change the color of each SpriteVisual
    for (auto &spriteVisual : m_spriteVisuals) {
      auto colorBrush = m_visual.Compositor().CreateColorBrush(color);
      // Set the new color brush on the SpriteVisual
      spriteVisual.FillBrush(colorBrush);
    }
  }

  typename TTypeRedirects::ShapeVisual createLoadingCircle(typename TTypeRedirects::Compositor compositor, int delay) {
    // Create circle
    auto ellipse = compositor.CreateEllipseGeometry();
    ellipse.Radius({m_ringWidth, m_ringWidth});
    auto spriteShape = compositor.CreateSpriteShape();
    spriteShape.Geometry(ellipse);
    spriteShape.Offset(winrt::Windows::Foundation::Numerics::float2(m_centerX, m_centerY + m_radiusSmall));
    auto spriteVisualBrush = compositor.CreateColorBrush(winrt::Windows::UI::Colors::LightGray());
    spriteShape.FillBrush(spriteVisualBrush);
    auto circleShape = compositor.CreateShapeVisual();
    circleShape.Shapes().Append(spriteShape);
    circleShape.Size({100.0f, 100.0f});
    circleShape.Opacity(0.0f);
    m_spriteVisuals.push_back(spriteShape);

    // Create an OpacityAnimation
    auto opacityAnimation = compositor.CreateScalarKeyFrameAnimation();
    opacityAnimation.Duration(std::chrono::seconds(2));
    opacityAnimation.IterationBehavior(TTypeRedirects::AnimationIterationBehavior::Forever);
    opacityAnimation.DelayTime(std::chrono::milliseconds(650 + delay));

    opacityAnimation.InsertKeyFrame(0.0f, 0.0f); // Initial opacity (fully transparent)
    opacityAnimation.InsertKeyFrame(0.5f, 1.0f); // Intermediate opacity (fully opaque)
    opacityAnimation.InsertKeyFrame(1.0f, 0.0f); // Intermediate opacity (fully opaque)
    opacityAnimation.InsertKeyFrame(0.0f, 0.0f); // Final opacity (fully transparent)

    // create an animation for the Offset property of the ShapeVisual
    auto animation = compositor.CreateVector2KeyFrameAnimation();
    animation.Duration(std::chrono::seconds(2));
    animation.Direction(TTypeRedirects::AnimationDirection::Normal);
    animation.IterationBehavior(TTypeRedirects::AnimationIterationBehavior::Forever);
    animation.DelayTime(std::chrono::milliseconds(delay));
    animation.DelayBehavior(TTypeRedirects::AnimationDelayBehavior::SetInitialValueAfterDelay);

    // create path animation
    float progress = 2.0f * static_cast<float>(M_PI); // specifies the end of the keyframe progress
    for (float angle = 0.0f; angle < progress; angle += 0.1f) {
      float x = m_centerX + m_radiusSmall * cos(angle);
      float y = m_centerY + m_radiusSmall * sin(angle);
      animation.InsertKeyFrame(
          angle / (2.0f * static_cast<float>(M_PI)), winrt::Windows::Foundation::Numerics::float2(x, y));
    }

    // run animations
    circleShape.StartAnimation(L"Opacity", opacityAnimation);
    spriteShape.StartAnimation(L"Offset", animation);

    return circleShape;
  }

  typename TTypeRedirects::Visual InnerVisual() const noexcept {
    return m_visual;
  }

  void InsertAt(const winrt::Microsoft::ReactNative::Composition::IVisual &visual, uint32_t index) noexcept {
    auto containerChildren = m_contentVisual.Children();
    auto compVisual = typename TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    if (index == 0 || containerChildren.Count() == 0) {
      containerChildren.InsertAtTop(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(const winrt::Microsoft::ReactNative::Composition::IVisual &visual) noexcept {
    auto compVisual = typename TTypeRedirects::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_contentVisual.Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual GetAt(uint32_t index) noexcept {
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
    m_visualSize = size;
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

  winrt::Microsoft::ReactNative::Composition::BackfaceVisibility BackfaceVisibility() {
    return static_cast<winrt::Microsoft::ReactNative::Composition::BackfaceVisibility>(m_visual.BackfaceVisibility());
  }

  void BackfaceVisibility(winrt::Microsoft::ReactNative::Composition::BackfaceVisibility value) {
    m_visual.BackfaceVisibility(static_cast<typename TTypeRedirects::CompositionBackfaceVisibility>(value));
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

 private:
  winrt::Windows::Foundation::Numerics::float2 m_visualSize{0};
  typename TTypeRedirects::SpriteVisual m_visual{nullptr};
  typename TTypeRedirects::SpriteVisual m_contentVisual{nullptr};
  std::vector<typename TTypeRedirects::CompositionSpriteShape> m_spriteVisuals;

  // constants
  float m_radiusSmall = 8.0f;
  float m_radiusLarge = 16.0f;
  float m_ringWidth = 2.0f;
  float m_centerX = m_radiusSmall + m_ringWidth;
  float m_centerY = m_radiusSmall + m_ringWidth;
};
using WindowsCompActivityVisual = CompActivityVisual<WindowsTypeRedirects>;
#ifdef USE_WINUI3
using MicrosoftCompActivityVisual = CompActivityVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompCaretVisual
    : winrt::implements<CompCaretVisual<TTypeRedirects>, winrt::Microsoft::ReactNative::Composition::ICaretVisual> {
  CompCaretVisual(typename TTypeRedirects::Compositor const &compositor)
      : m_compositor(compositor),
        m_compVisual(compositor.CreateSpriteVisual()),
        m_opacityAnimation(compositor.CreateScalarKeyFrameAnimation()) {
    m_visual = CreateVisual();

    // TODO should make the caret use an invert brush by default
    m_compVisual.Brush(m_compositor.CreateColorBrush(winrt::Windows::UI::Colors::Black()));
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

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() const noexcept;

  winrt::Microsoft::ReactNative::Composition::IVisual InnerVisual() const noexcept {
    return m_visual;
  }

  void Color(winrt::Windows::UI::Color color) noexcept {
    m_compVisual.Brush(m_compositor.CreateColorBrush(color));
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

 private:
  bool m_isVisible{false};
  typename TTypeRedirects::SpriteVisual m_compVisual;
  winrt::Microsoft::ReactNative::Composition::IVisual m_visual;
  typename TTypeRedirects::ScalarKeyFrameAnimation m_opacityAnimation;
  typename TTypeRedirects::Compositor m_compositor{nullptr};
};

winrt::Microsoft::ReactNative::Composition::IVisual CompCaretVisual<WindowsTypeRedirects>::CreateVisual()
    const noexcept {
  return winrt::make<Composition::WindowsCompSpriteVisual>(m_compVisual);
}
using WindowsCompCaretVisual = CompCaretVisual<WindowsTypeRedirects>;

#ifdef USE_WINUI3
winrt::Microsoft::ReactNative::Composition::IVisual CompCaretVisual<MicrosoftTypeRedirects>::CreateVisual()
    const noexcept {
  return winrt::make<Composition::MicrosoftCompSpriteVisual>(m_compVisual);
}
using MicrosoftCompCaretVisual = CompCaretVisual<MicrosoftTypeRedirects>;
#endif

// Switch Thumb animations
template <typename TTypeRedirects>
struct CompSwitchThumbVisual : winrt::implements<
                                   CompSwitchThumbVisual<TTypeRedirects>,
                                   winrt::Microsoft::ReactNative::Composition::ISwitchThumbVisual> {
  CompSwitchThumbVisual(typename TTypeRedirects::Compositor const &compositor)
      : m_compositor(compositor), m_compVisual(compositor.CreateSpriteVisual()) {
    m_visual = CreateVisual();

    // Create Thumb
    m_geometry = m_compositor.CreateEllipseGeometry();
    m_spiritShape = m_compositor.CreateSpriteShape();
    m_spiritShape.Geometry(m_geometry);
    auto circleShape = m_compositor.CreateShapeVisual();
    circleShape.Shapes().Append(m_spiritShape);
    circleShape.Size({150.0f, 150.0f});

    m_compVisual.Children().InsertAtTop(circleShape);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() const noexcept;

  winrt::Microsoft::ReactNative::Composition::IVisual InnerVisual() const noexcept {
    return m_visual;
  }

  void Color(winrt::Windows::UI::Color color) noexcept {
    m_spiritShape.FillBrush(m_compositor.CreateColorBrush(color));
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 size) noexcept {
    m_geometry.Radius(size);
    m_spiritShape.Offset(size);
    m_compVisual.Size(size);
  }

  void Position(winrt::Windows::Foundation::Numerics::float2 position) noexcept {
    if (!isDrawn) {
      // we don't want to animate if this is the first time the switch is drawn on screen
      isDrawn = true;
      m_compVisual.Offset({position.x, position.y, 0.0f});
    } else {
      auto animation = m_compositor.CreateVector3KeyFrameAnimation();
      animation.Duration(std::chrono::milliseconds(250));
      animation.Direction(TTypeRedirects::AnimationDirection::Normal);
      animation.InsertKeyFrame(1.0f, {position.x, position.y, 0.0f});

      m_compVisual.StartAnimation(L"Offset", animation);
    }
  }

  bool IsVisible() const noexcept {
    return m_isVisible;
  }

  void IsVisible(bool value) noexcept {}

 private:
  bool m_isVisible{true};
  bool isDrawn{false};
  typename TTypeRedirects::SpriteVisual m_compVisual;
  winrt::Microsoft::ReactNative::Composition::IVisual m_visual;
  typename TTypeRedirects::Compositor m_compositor{nullptr};
  typename TTypeRedirects::CompositionSpriteShape m_spiritShape{nullptr};
  typename TTypeRedirects::CompositionEllipseGeometry m_geometry{nullptr};
};

winrt::Microsoft::ReactNative::Composition::IVisual CompSwitchThumbVisual<WindowsTypeRedirects>::CreateVisual()
    const noexcept {
  return winrt::make<Composition::WindowsCompSpriteVisual>(m_compVisual);
}
using WindowsCompSwitchThumbVisual = CompSwitchThumbVisual<WindowsTypeRedirects>;

#ifdef USE_WINUI3
winrt::Microsoft::ReactNative::Composition::IVisual CompSwitchThumbVisual<MicrosoftTypeRedirects>::CreateVisual()
    const noexcept {
  return winrt::make<Composition::MicrosoftCompSpriteVisual>(m_compVisual);
}
using MicrosoftCompSwitchThumbVisual = CompSwitchThumbVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompFocusVisual
    : winrt::implements<CompFocusVisual<TTypeRedirects>, winrt::Microsoft::ReactNative::Composition::IFocusVisual> {
  CompFocusVisual(typename TTypeRedirects::Compositor const &compositor)
      : m_compVisual(compositor.CreateSpriteVisual()), m_brush(compositor.CreateNineGridBrush()) {
    m_visual = CreateVisual();

    m_compVisual.Opacity(1.0f);
    m_compVisual.RelativeSizeAdjustment({1, 1});

    m_brush.Source(compositor.CreateColorBrush(winrt::Windows::UI::Colors::Black()));
    m_brush.IsCenterHollow(true);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::IVisual InnerVisual() const noexcept {
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

 private:
  float m_scaleFactor{0};
  typename TTypeRedirects::CompositionNineGridBrush m_brush;
  typename TTypeRedirects::SpriteVisual m_compVisual;
  winrt::Microsoft::ReactNative::Composition::IVisual m_visual{nullptr};
};

winrt::Microsoft::ReactNative::Composition::IVisual CompFocusVisual<WindowsTypeRedirects>::CreateVisual() noexcept {
  return winrt::make<Composition::WindowsCompSpriteVisual>(m_compVisual);
}
using WindowsCompFocusVisual = CompFocusVisual<WindowsTypeRedirects>;

#ifdef USE_WINUI3
winrt::Microsoft::ReactNative::Composition::IVisual CompFocusVisual<MicrosoftTypeRedirects>::CreateVisual() noexcept {
  return winrt::make<Composition::MicrosoftCompSpriteVisual>(m_compVisual);
}
using MicrosoftCompFocusVisual = CompFocusVisual<MicrosoftTypeRedirects>;
#endif

template <typename TTypeRedirects>
struct CompContext : winrt::implements<
                         CompContext<TTypeRedirects>,
                         winrt::Microsoft::ReactNative::Composition::ICompositionContext,
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

  winrt::Microsoft::ReactNative::Composition::ISpriteVisual CreateSpriteVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::IScrollVisual CreateScrollerVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::IActivityVisual CreateActivityVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::IDropShadow CreateDropShadow() noexcept;

  winrt::Microsoft::ReactNative::Composition::IBrush CreateColorBrush(winrt::Windows::UI::Color color) noexcept;

  winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush CreateDrawingSurfaceBrush(
      winrt::Windows::Foundation::Size surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode) noexcept;

  winrt::Microsoft::ReactNative::Composition::ICaretVisual CreateCaretVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::ISwitchThumbVisual CreateSwitchThumbVisual() noexcept;

  winrt::Microsoft::ReactNative::Composition::IFocusVisual CreateFocusVisual() noexcept;

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

winrt::Microsoft::ReactNative::Composition::ISpriteVisual
CompContext<WindowsTypeRedirects>::CreateSpriteVisual() noexcept {
  return winrt::make<Composition::WindowsCompSpriteVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::IScrollVisual
CompContext<WindowsTypeRedirects>::CreateScrollerVisual() noexcept {
  return winrt::make<Composition::WindowsCompScrollerVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::IActivityVisual
CompContext<WindowsTypeRedirects>::CreateActivityVisual() noexcept {
  return winrt::make<Composition::WindowsCompActivityVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::IDropShadow CompContext<WindowsTypeRedirects>::CreateDropShadow() noexcept {
  return winrt::make<Composition::WindowsCompDropShadow>(m_compositor.CreateDropShadow());
}

winrt::Microsoft::ReactNative::Composition::IBrush CompContext<WindowsTypeRedirects>::CreateColorBrush(
    winrt::Windows::UI::Color color) noexcept {
  return winrt::make<Composition::WindowsCompBrush>(m_compositor.CreateColorBrush(color));
}

winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush
CompContext<WindowsTypeRedirects>::CreateDrawingSurfaceBrush(
    winrt::Windows::Foundation::Size surfaceSize,
    winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
    winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode) noexcept {
  return winrt::make<Composition::WindowsCompDrawingSurfaceBrush>(
      m_compositor, CompositionGraphicsDevice().CreateDrawingSurface(surfaceSize, pixelFormat, alphaMode));
}

winrt::Microsoft::ReactNative::Composition::ICaretVisual
CompContext<WindowsTypeRedirects>::CreateCaretVisual() noexcept {
  return winrt::make<Composition::WindowsCompCaretVisual>(m_compositor);
}

winrt::Microsoft::ReactNative::Composition::ISwitchThumbVisual
CompContext<WindowsTypeRedirects>::CreateSwitchThumbVisual() noexcept {
  return winrt::make<Composition::WindowsCompSwitchThumbVisual>(m_compositor);
}

winrt::Microsoft::ReactNative::Composition::IFocusVisual
CompContext<WindowsTypeRedirects>::CreateFocusVisual() noexcept {
  return winrt::make<Composition::WindowsCompFocusVisual>(m_compositor);
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
winrt::Microsoft::ReactNative::Composition::ISpriteVisual
CompContext<MicrosoftTypeRedirects>::CreateSpriteVisual() noexcept {
  return winrt::make<Composition::MicrosoftCompSpriteVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::IScrollVisual
CompContext<MicrosoftTypeRedirects>::CreateScrollerVisual() noexcept {
  return winrt::make<Composition::MicrosoftCompScrollerVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::IActivityVisual
CompContext<MicrosoftTypeRedirects>::CreateActivityVisual() noexcept {
  return winrt::make<Composition::MicrosoftCompActivityVisual>(m_compositor.CreateSpriteVisual());
}

winrt::Microsoft::ReactNative::Composition::IDropShadow
CompContext<MicrosoftTypeRedirects>::CreateDropShadow() noexcept {
  return winrt::make<Composition::MicrosoftCompDropShadow>(m_compositor.CreateDropShadow());
}

winrt::Microsoft::ReactNative::Composition::IBrush CompContext<MicrosoftTypeRedirects>::CreateColorBrush(
    winrt::Windows::UI::Color color) noexcept {
  return winrt::make<Composition::MicrosoftCompBrush>(m_compositor.CreateColorBrush(color));
}

winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush
CompContext<MicrosoftTypeRedirects>::CreateDrawingSurfaceBrush(
    winrt::Windows::Foundation::Size surfaceSize,
    winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
    winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode) noexcept {
  return winrt::make<Composition::MicrosoftCompDrawingSurfaceBrush>(
      m_compositor,
      CompositionGraphicsDevice().CreateDrawingSurface(
          surfaceSize,
          static_cast<winrt::Microsoft::Graphics::DirectX::DirectXPixelFormat>(pixelFormat),
          static_cast<winrt::Microsoft::Graphics::DirectX::DirectXAlphaMode>(alphaMode)));
}

winrt::Microsoft::ReactNative::Composition::ICaretVisual
CompContext<MicrosoftTypeRedirects>::CreateCaretVisual() noexcept {
  return winrt::make<Composition::MicrosoftCompCaretVisual>(m_compositor);
}

winrt::Microsoft::ReactNative::Composition::ISwitchThumbVisual
CompContext<MicrosoftTypeRedirects>::CreateSwitchThumbVisual() noexcept {
  return winrt::make<Composition::MicrosoftCompSwitchThumbVisual>(m_compositor);
}

winrt::Microsoft::ReactNative::Composition::IFocusVisual
CompContext<MicrosoftTypeRedirects>::CreateFocusVisual() noexcept {
  return winrt::make<Composition::MicrosoftCompFocusVisual>(m_compositor);
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

} // namespace Microsoft::ReactNative::Composition

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ICompositionContext WindowsCompositionContextHelper::CreateContext(
    winrt::Windows::UI::Composition::Compositor const &compositor) noexcept {
  return winrt::make<::Microsoft::ReactNative::Composition::WindowsCompContext>(compositor);
}

IVisual WindowsCompositionContextHelper::CreateVisual(winrt::Windows::UI::Composition::Visual const &visual) noexcept {
  if (auto spriteVisual = visual.try_as<winrt::Windows::UI::Composition::SpriteVisual>())
    return winrt::make<::Microsoft::ReactNative::Composition::WindowsCompSpriteVisual>(spriteVisual);
  return winrt::make<::Microsoft::ReactNative::Composition::WindowsCompVisual>(visual);
}

winrt::Windows::UI::Composition::Compositor WindowsCompositionContextHelper::InnerCompositor(
    ICompositionContext context) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionCompositor> s;
  context.try_as(s);
  return s ? s->InnerCompositor() : nullptr;
}

winrt::Windows::UI::Composition::Visual WindowsCompositionContextHelper::InnerVisual(IVisual visual) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionVisual> s;
  visual.try_as(s);
  return s ? s->InnerVisual() : nullptr;
}

winrt::Windows::UI::Composition::DropShadow WindowsCompositionContextHelper::InnerDropShadow(
    IDropShadow shadow) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionDropShadow> s;
  shadow.try_as(s);
  return s ? s->InnerShadow() : nullptr;
}

winrt::Windows::UI::Composition::CompositionBrush WindowsCompositionContextHelper::InnerBrush(IBrush brush) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionBrush> s;
  brush.try_as(s);
  return s ? s->InnerBrush() : nullptr;
}

winrt::Windows::UI::Composition::ICompositionSurface WindowsCompositionContextHelper::InnerSurface(
    IDrawingSurfaceBrush surface) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::IWindowsCompositionDrawingSurfaceInner> s;
  surface.try_as(s);
  return s ? s->Inner() : nullptr;
}

#ifdef USE_WINUI3
ICompositionContext MicrosoftCompositionContextHelper::CreateContext(
    winrt::Microsoft::UI::Composition::Compositor const &compositor) noexcept {
  return winrt::make<::Microsoft::ReactNative::Composition::MicrosoftCompContext>(compositor);
}

IVisual MicrosoftCompositionContextHelper::CreateVisual(
    winrt::Microsoft::UI::Composition::Visual const &visual) noexcept {
  if (auto spriteVisual = visual.try_as<winrt::Microsoft::UI::Composition::SpriteVisual>())
    return winrt::make<::Microsoft::ReactNative::Composition::MicrosoftCompSpriteVisual>(spriteVisual);
  return winrt::make<::Microsoft::ReactNative::Composition::MicrosoftCompVisual>(visual);
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
#endif

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
