
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.CompositionContextHelper.g.cpp")
#include "Composition.CompositionContextHelper.g.cpp"
#endif

#include <Composition.ScrollPositionChangedArgs.g.h>
#include <Composition.ScrollVisual.g.h>
#include <Composition.SpriteVisual.g.h>
#include <Composition.SurfaceBrush.g.h>
#include <Windows.Graphics.Interop.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.interactions.h>
#include "CompositionHelpers.h"

namespace Microsoft::ReactNative::Composition {

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

struct CompositionDrawingSurface : public winrt::implements<
                                       CompositionDrawingSurface,
                                       winrt::Microsoft::ReactNative::Composition::ICompositionDrawingSurface,
                                       ICompositionDrawingSurfaceInterop,
                                       ICompositionDrawingSurfaceInner> {
  CompositionDrawingSurface(winrt::Windows::UI::Composition::CompositionDrawingSurface const &drawingSurface) {
    drawingSurface.as(m_drawingSurfaceInterop);
  }

  HRESULT BeginDraw(ID2D1DeviceContext **deviceContextOut, POINT *offset) noexcept {
    return m_drawingSurfaceInterop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), (void **)deviceContextOut, offset);
  }

  HRESULT EndDraw() noexcept {
    return m_drawingSurfaceInterop->EndDraw();
  }

  winrt::Windows::UI::Composition::ICompositionSurface Inner() const noexcept {
    winrt::Windows::UI::Composition::ICompositionSurface surface;
    m_drawingSurfaceInterop.as(surface);
    return surface;
  }

 private:
  winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop> m_drawingSurfaceInterop;
};

struct CompDropShadow
    : public winrt::
          implements<CompDropShadow, winrt::Microsoft::ReactNative::Composition::IDropShadow, ICompositionDropShadow> {
  CompDropShadow(winrt::Windows::UI::Composition::DropShadow const &shadow) : m_shadow(shadow) {}

  winrt::Windows::UI::Composition::DropShadow InnerShadow() const noexcept override {
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
  winrt::Windows::UI::Composition::DropShadow m_shadow;
};

struct CompBrush
    : public winrt::implements<CompBrush, winrt::Microsoft::ReactNative::Composition::IBrush, ICompositionBrush> {
  CompBrush(winrt::Windows::UI::Composition::CompositionBrush const &brush) : m_brush(brush) {}

  winrt::Windows::UI::Composition::CompositionBrush InnerBrush() const noexcept {
    return m_brush;
  }

 private:
  winrt::Windows::UI::Composition::CompositionBrush m_brush;
};

struct CompSurfaceBrush
    : winrt::Microsoft::ReactNative::Composition::implementation::SurfaceBrushT<CompSurfaceBrush, ICompositionBrush> {
  CompSurfaceBrush(
      const winrt::Windows::UI::Composition::Compositor &compositor,
      const winrt::Microsoft::ReactNative::Composition::ICompositionDrawingSurface &drawingSurfaceInterop)
      : m_brush(compositor.CreateSurfaceBrush(
            winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerSurface(
                drawingSurfaceInterop))) {}

  winrt::Windows::UI::Composition::CompositionBrush InnerBrush() const noexcept {
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
            winrt::Windows::UI::Composition::CompositionStretch::None) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::CompositionStretch>(
            winrt::Windows::UI::Composition::CompositionStretch::Fill) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::Fill);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::CompositionStretch>(
            winrt::Windows::UI::Composition::CompositionStretch::Uniform) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::Uniform);
    static_assert(
        static_cast<winrt::Microsoft::ReactNative::Composition::CompositionStretch>(
            winrt::Windows::UI::Composition::CompositionStretch::UniformToFill) ==
        winrt::Microsoft::ReactNative::Composition::CompositionStretch::UniformToFill);

    m_brush.Stretch(static_cast<winrt::Windows::UI::Composition::CompositionStretch>(mode));
  }

 private:
  winrt::Windows::UI::Composition::CompositionSurfaceBrush m_brush;
};

struct CompVisual : public winrt::implements<
                        CompVisual,
                        winrt::Microsoft::ReactNative::Composition::IVisual,
                        ICompositionVisual,
                        IVisualInterop> {
  CompVisual(winrt::Windows::UI::Composition::Visual const &visual) : m_visual(visual) {}

  winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept override {
    return m_visual;
  }

  void InsertAt(const winrt::Microsoft::ReactNative::Composition::IVisual &visual, uint32_t index) noexcept {
    auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    auto compVisual = winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerVisual(visual);
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
    auto compVisual = winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::CreateVisual(
        it.Current());
  }

  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept {
    auto geometry = winrt::make<GeometrySource>(clippingPath);
    auto path = winrt::Windows::UI::Composition::CompositionPath(geometry);
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

 private:
  winrt::Windows::UI::Composition::Visual m_visual;
};

struct CompSpriteVisual : winrt::Microsoft::ReactNative::Composition::implementation::
                              SpriteVisualT<CompSpriteVisual, ICompositionVisual, IVisualInterop> {
  CompSpriteVisual(winrt::Windows::UI::Composition::SpriteVisual const &visual) : m_visual(visual) {}

  winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept override {
    return m_visual;
  }

  void InsertAt(const winrt::Microsoft::ReactNative::Composition::IVisual &visual, uint32_t index) noexcept {
    auto containerChildren = m_visual.Children();
    auto compVisual = winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerVisual(visual);
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
    auto compVisual = winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_visual.Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::CreateVisual(
        it.Current());
  }

  void Brush(const winrt::Microsoft::ReactNative::Composition::IBrush &brush) noexcept {
    m_visual.Brush(winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerBrush(brush));
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

  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept {
    if (!clippingPath) {
      m_visual.Clip(nullptr);
      return;
    }
    auto geometry = winrt::make<GeometrySource>(clippingPath);
    auto path = winrt::Windows::UI::Composition::CompositionPath(geometry);
    auto pathgeo = m_visual.Compositor().CreatePathGeometry(path);
    auto clip = m_visual.Compositor().CreateGeometricClip(pathgeo);
    m_visual.Clip(clip);
  }

  void Shadow(const winrt::Microsoft::ReactNative::Composition::IDropShadow &shadow) noexcept {
    m_visual.Shadow(winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerDropShadow(shadow));
  }

 private:
  winrt::Windows::UI::Composition::SpriteVisual m_visual;
};

struct CompScrollPositionChangedArgs
    : winrt::Microsoft::ReactNative::Composition::implementation::ScrollPositionChangedArgsT<
          CompScrollPositionChangedArgs> {
  CompScrollPositionChangedArgs(winrt::Windows::Foundation::Numerics::float2 position) : m_position(position) {}

  winrt::Windows::Foundation::Numerics::float2 Position() {
    return m_position;
  }

 private:
  winrt::Windows::Foundation::Numerics::float2 m_position;
};

struct CompScrollerVisual : winrt::Microsoft::ReactNative::Composition::implementation::
                                ScrollVisualT<CompScrollerVisual, ICompositionVisual, IVisualInterop> {
  struct ScrollInteractionTrackerOwner : public winrt::implements<
                                             ScrollInteractionTrackerOwner,
                                             winrt::Windows::UI::Composition::Interactions::IInteractionTrackerOwner> {
    ScrollInteractionTrackerOwner(CompScrollerVisual *outer) : m_outer(outer){};

    void CustomAnimationStateEntered(
        winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
        winrt::Windows::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs
            args) noexcept {}
    void IdleStateEntered(
        winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
        winrt::Windows::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs args) noexcept {}
    void InertiaStateEntered(
        winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
        winrt::Windows::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs args) noexcept {}
    void InteractingStateEntered(
        winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
        winrt::Windows::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs args) noexcept {}
    void RequestIgnored(
        winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
        winrt::Windows::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs args) noexcept {}
    void ValuesChanged(
        winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
        winrt::Windows::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs args) noexcept {
      m_outer->FireScrollPositionChanged({args.Position().x, args.Position().y});
    }

   private:
    CompScrollerVisual *m_outer;
  };

  CompScrollerVisual(winrt::Windows::UI::Composition::SpriteVisual const &visual) : m_visual(visual) {
    auto compositor = m_visual.Compositor();
    m_contentVisual = compositor.CreateSpriteVisual();

    auto brush = compositor.CreateColorBrush({0, 0, 0, 0}); // transparent brush so that hit testing works
    m_visual.Brush(brush);
    m_contentVisual.Brush(brush);

    m_visual.Children().InsertAtTop(m_contentVisual);

    m_visual.Clip(compositor.CreateInsetClip(0, 0, 0, 0));

    m_interactionTracker = winrt::Windows::UI::Composition::Interactions::InteractionTracker::CreateWithOwner(
        compositor, winrt::make<ScrollInteractionTrackerOwner>(this));

    m_interactionTracker.MinScale(1.0);
    m_interactionTracker.MaxScale(1.0);

    m_visualInteractionSource =
        winrt::Windows::UI::Composition::Interactions::VisualInteractionSource::Create(m_visual);

    m_visualInteractionSource.PositionXSourceMode(
        winrt::Windows::UI::Composition::Interactions::InteractionSourceMode::EnabledWithInertia);
    m_visualInteractionSource.PositionYSourceMode(
        winrt::Windows::UI::Composition::Interactions::InteractionSourceMode::EnabledWithInertia);
    m_visualInteractionSource.ScaleSourceMode(
        winrt::Windows::UI::Composition::Interactions::InteractionSourceMode::Disabled);

    m_visualInteractionSource.ManipulationRedirectionMode(
        winrt::Windows::UI::Composition::Interactions::VisualInteractionSourceRedirectionMode::
            CapableTouchpadAndPointerWheel);
    m_interactionTracker.InteractionSources().Add(m_visualInteractionSource);

    auto positionExpression = compositor.CreateExpressionAnimation(L"-tracker.Position");
    positionExpression.SetReferenceParameter(L"tracker", m_interactionTracker);
    m_contentVisual.StartAnimation(L"Offset", positionExpression);
  }

  winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept {
    return m_visual;
  }

  void InsertAt(const winrt::Microsoft::ReactNative::Composition::IVisual &visual, uint32_t index) noexcept {
    auto containerChildren = m_contentVisual.Children();
    auto compVisual = winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerVisual(visual);
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
    auto compVisual = winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerVisual(visual);
    auto containerChildren = m_contentVisual.Children();
    containerChildren.Remove(compVisual);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual GetAt(uint32_t index) noexcept {
    auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    auto it = containerChildren.First();
    for (uint32_t i = 0; i < index; i++)
      it.MoveNext();
    return winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::CreateVisual(
        it.Current());
  }

  void Brush(const winrt::Microsoft::ReactNative::Composition::IBrush &brush) noexcept {
    m_visual.Brush(winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerBrush(brush));
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

  void SetClippingPath(ID2D1Geometry *clippingPath) noexcept {
    auto geometry = winrt::make<GeometrySource>(clippingPath);
    auto path = winrt::Windows::UI::Composition::CompositionPath(geometry);
    auto pathgeo = m_visual.Compositor().CreatePathGeometry(path);
    auto clip = m_visual.Compositor().CreateGeometricClip(pathgeo);
    m_visual.Clip(clip);
  }

  void Shadow(const winrt::Microsoft::ReactNative::Composition::IDropShadow &shadow) noexcept {
    m_visual.Shadow(winrt::Microsoft::ReactNative::Composition::CompositionContextHelper::InnerDropShadow(shadow));
  }

  winrt::event_token ScrollPositionChanged(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::ScrollPositionChangedArgs> const &handler) {
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

  void ScrollBy(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept {
    m_interactionTracker.TryUpdatePositionBy(offset);
  };

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

  winrt::Windows::Foundation::Numerics::float2 m_contentSize{0};
  winrt::Windows::Foundation::Numerics::float2 m_visualSize{0};
  winrt::event<
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::ScrollPositionChangedArgs>>
      m_scrollPositionChangedEvent;
  winrt::Windows::UI::Composition::SpriteVisual m_visual{nullptr};
  winrt::Windows::UI::Composition::SpriteVisual m_contentVisual{nullptr};
  winrt::Windows::UI::Composition::Interactions::InteractionTracker m_interactionTracker{nullptr};
  winrt::Windows::UI::Composition::Interactions::VisualInteractionSource m_visualInteractionSource{nullptr};
};

struct CompCaretVisual : winrt::implements<CompCaretVisual, winrt::Microsoft::ReactNative::Composition::ICaretVisual> {
  CompCaretVisual(winrt::Windows::UI::Composition::Compositor const &compositor)
      : m_compositor(compositor),
        m_compVisual(compositor.CreateSpriteVisual()),
        m_opacityAnimation(compositor.CreateScalarKeyFrameAnimation()) {
    m_visual = winrt::make<Composition::CompSpriteVisual>(m_compVisual);

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
    m_opacityAnimation.IterationBehavior(winrt::Windows::UI::Composition::AnimationIterationBehavior::Count);
    m_opacityAnimation.IterationCount(500);
  }

  winrt::Microsoft::ReactNative::Composition::IVisual InnerVisual() const noexcept {
    return m_visual;
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
    if (value) {
      m_compVisual.StartAnimation(L"opacity", m_opacityAnimation);
    } else {
      m_compVisual.StopAnimation(L"opacity");
    }
  }

 private:
  bool m_isVisible{false};
  winrt::Windows::UI::Composition::SpriteVisual m_compVisual;
  winrt::Microsoft::ReactNative::Composition::IVisual m_visual;
  winrt::Windows::UI::Composition::ScalarKeyFrameAnimation m_opacityAnimation;
  winrt::Windows::UI::Composition::Compositor m_compositor{nullptr};
};

struct CompContext : winrt::implements<
                         CompContext,
                         winrt::Microsoft::ReactNative::Composition::ICompositionContext,
                         ICompositionContextInterop> {
  CompContext(winrt::Windows::UI::Composition::Compositor const &compositor) : m_compositor(compositor) {}

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

  winrt::Microsoft::ReactNative::Composition::ICompositionDrawingSurface CreateDrawingSurface(
      winrt::Windows::Foundation::Size surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode) noexcept {
    return winrt::make<Composition::CompositionDrawingSurface>(
        CompositionGraphicsDevice().CreateDrawingSurface(surfaceSize, pixelFormat, alphaMode));
  }

  winrt::Microsoft::ReactNative::Composition::SpriteVisual CreateSpriteVisual() noexcept {
    return winrt::make<Composition::CompSpriteVisual>(m_compositor.CreateSpriteVisual());
  }

  winrt::Microsoft::ReactNative::Composition::ScrollVisual CreateScrollerVisual() noexcept {
    return winrt::make<Composition::CompScrollerVisual>(m_compositor.CreateSpriteVisual());
  }

  winrt::Microsoft::ReactNative::Composition::IDropShadow CreateDropShadow() noexcept {
    return winrt::make<Composition::CompDropShadow>(m_compositor.CreateDropShadow());
  }

  winrt::Microsoft::ReactNative::Composition::IBrush CreateColorBrush(winrt::Windows::UI::Color color) noexcept {
    return winrt::make<Composition::CompBrush>(m_compositor.CreateColorBrush(color));
  }

  winrt::Microsoft::ReactNative::Composition::SurfaceBrush CreateSurfaceBrush(
      const winrt::Microsoft::ReactNative::Composition::ICompositionDrawingSurface &surface) noexcept {
    return winrt::make<Composition::CompSurfaceBrush>(m_compositor, surface);
  }

  winrt::Microsoft::ReactNative::Composition::ICaretVisual CreateCaretVisual() noexcept {
    return winrt::make<Composition::CompCaretVisual>(m_compositor);
  }

  winrt::Windows::UI::Composition::CompositionGraphicsDevice CompositionGraphicsDevice() noexcept {
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

  winrt::Windows::UI::Composition::Compositor InnerCompositor() noexcept {
    return m_compositor;
  }

 private:
  winrt::Windows::UI::Composition::Compositor m_compositor{nullptr};
  winrt::com_ptr<ID2D1Factory1> m_d2dFactory;
  winrt::com_ptr<ID3D11Device> m_d3dDevice;
  winrt::com_ptr<ID2D1Device> m_d2dDevice;
  winrt::Windows::UI::Composition::CompositionGraphicsDevice m_compositionGraphicsDevice{nullptr};
  winrt::com_ptr<ID3D11DeviceContext> m_d3dDeviceContext;
};

} // namespace Microsoft::ReactNative::Composition

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ICompositionContext CompositionContextHelper::CreateContext(
    winrt::Windows::UI::Composition::Compositor const &compositor) noexcept {
  return winrt::make<::Microsoft::ReactNative::Composition::CompContext>(compositor);
}

IVisual CompositionContextHelper::CreateVisual(winrt::Windows::UI::Composition::Visual const &visual) noexcept {
  if (auto spriteVisual = visual.try_as<winrt::Windows::UI::Composition::SpriteVisual>())
    return winrt::make<::Microsoft::ReactNative::Composition::CompSpriteVisual>(spriteVisual);
  return winrt::make<::Microsoft::ReactNative::Composition::CompVisual>(visual);
}

winrt::Windows::UI::Composition::Compositor CompositionContextHelper::InnerCompositor(
    ICompositionContext context) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::CompContext> s;
  context.as(s);
  return s ? s->InnerCompositor() : nullptr;
}

winrt::Windows::UI::Composition::Visual CompositionContextHelper::InnerVisual(IVisual visual) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::ICompositionVisual> s;
  visual.as(s);
  return s ? s->InnerVisual() : nullptr;
}

winrt::Windows::UI::Composition::DropShadow CompositionContextHelper::InnerDropShadow(IDropShadow shadow) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::ICompositionDropShadow> s;
  shadow.as(s);
  return s ? s->InnerShadow() : nullptr;
}

winrt::Windows::UI::Composition::CompositionBrush CompositionContextHelper::InnerBrush(IBrush brush) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::ICompositionBrush> s;
  brush.as(s);
  return s ? s->InnerBrush() : nullptr;
}

winrt::Windows::UI::Composition::ICompositionSurface CompositionContextHelper::InnerSurface(
    ICompositionDrawingSurface surface) noexcept {
  winrt::com_ptr<::Microsoft::ReactNative::Composition::ICompositionDrawingSurfaceInner> s;
  surface.as(s);
  return s ? s->Inner() : nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
