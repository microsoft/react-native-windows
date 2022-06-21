
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.CompositionContextHelper.g.cpp")
#include "Composition.CompositionContextHelper.g.cpp"
#endif

#include <windows.ui.composition.interop.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.interactions.h>
#include "CompHelpers.h"

namespace Microsoft::ReactNative::Composition {

struct CompositionDrawingSurface
    : public winrt::implements<CompositionDrawingSurface, ICompositionDrawingSurface, ICompositionDrawingSurfaceInner> {
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

struct CompDropShadow : public winrt::implements<CompDropShadow, IDropShadow, ICompositionDropShadow> {
  CompDropShadow(winrt::Windows::UI::Composition::DropShadow const &shadow) : m_shadow(shadow) {}

  winrt::Windows::UI::Composition::DropShadow InnerShadow() const noexcept override {
    return m_shadow;
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept override {
    m_shadow.Offset(offset);
  }

  void Opacity(float opacity) noexcept override {
    m_shadow.Opacity(opacity);
  }

  void BlurRadius(float radius) noexcept override {
    m_shadow.BlurRadius(radius);
  }

  void Color(winrt::Windows::UI::Color color) noexcept override {
    m_shadow.Color(color);
  }

 private:
  winrt::Windows::UI::Composition::DropShadow m_shadow;
};

struct CompBrush : public winrt::implements<CompBrush, IBrush, ICompositionBrush> {
  CompBrush(winrt::Windows::UI::Composition::CompositionBrush const &brush) : m_brush(brush) {}

  winrt::Windows::UI::Composition::CompositionBrush InnerBrush() const noexcept {
    return m_brush;
  }

 private:
  winrt::Windows::UI::Composition::CompositionBrush m_brush;
};

struct CompSurfaceBrush : public winrt::implements<CompSurfaceBrush, ISurfaceBrush, ICompositionBrush> {
  CompSurfaceBrush(
      const winrt::Windows::UI::Composition::Compositor &compositor,
      ICompositionDrawingSurface *drawingSurfaceInterop)
      : m_brush(compositor.CreateSurfaceBrush(CompDrawingSurfaceFromDrawingSurface(drawingSurfaceInterop))) {}

  winrt::Windows::UI::Composition::CompositionBrush InnerBrush() const noexcept {
    return m_brush;
  }

  void HorizontalAlignmentRatio(float ratio) noexcept {
    m_brush.HorizontalAlignmentRatio(ratio);
  }
  void VerticalAlignmentRatio(float ratio) noexcept {
    m_brush.VerticalAlignmentRatio(ratio);
  }

  void Stretch(CompositionStretch mode) noexcept {
    static_assert(
        static_cast<CompositionStretch>(winrt::Windows::UI::Composition::CompositionStretch::None) ==
        CompositionStretch::None);
    static_assert(
        static_cast<CompositionStretch>(winrt::Windows::UI::Composition::CompositionStretch::Fill) ==
        CompositionStretch::Fill);
    static_assert(
        static_cast<CompositionStretch>(winrt::Windows::UI::Composition::CompositionStretch::Uniform) ==
        CompositionStretch::Uniform);
    static_assert(
        static_cast<CompositionStretch>(winrt::Windows::UI::Composition::CompositionStretch::UniformToFill) ==
        CompositionStretch::UniformToFill);

    m_brush.Stretch(static_cast<winrt::Windows::UI::Composition::CompositionStretch>(mode));
  }

 private:
  winrt::Windows::UI::Composition::CompositionSurfaceBrush m_brush;
};

struct CompVisual : public winrt::implements<
                        CompVisual,
                        IVisual,
                        ICompositionVisual,
                        winrt::Microsoft::ReactNative::Composition::ICompositionVisual> {
  CompVisual(winrt::Windows::UI::Composition::Visual const &visual) : m_visual(visual) {}

  // Work around winrt ICompositionVisual not being able to be an empty interface
  int64_t Handle() const noexcept {
    return 0;
  }

  winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept override {
    return m_visual;
  }

  void InsertAt(IVisual *visual, uint32_t index) noexcept override {
    auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    auto compVisual = CompVisualFromVisual(visual);
    if (index == 0 || containerChildren.Count() == 0) {
      containerChildren.InsertAtTop(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(IVisual *visual) noexcept override {
    auto compVisual = CompVisualFromVisual(visual);
    auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    containerChildren.Remove(compVisual);
  }

  void Opacity(float opacity) noexcept override {
    m_visual.Opacity(opacity);
  }

  void Scale(winrt::Windows::Foundation::Numerics::float3 const &scale) noexcept override {
    m_visual.Scale(scale);
  }

  void RotationAngle(float rotation) noexcept override {
    m_visual.RotationAngle(rotation);
  }

  void IsVisible(bool isVisible) noexcept override {
    m_visual.IsVisible(isVisible);
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept override {
    m_visual.Size(size);
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept override {
    m_visual.Offset(offset);
  }

 private:
  winrt::Windows::UI::Composition::Visual m_visual;
};

struct CompSpriteVisual : public winrt::implements<CompSpriteVisual, ISpriteVisual, ICompositionVisual> {
  CompSpriteVisual(winrt::Windows::UI::Composition::SpriteVisual const &visual) : m_visual(visual) {}

  winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept override {
    return m_visual;
  }

  void InsertAt(IVisual *visual, uint32_t index) noexcept override {
    auto containerChildren = m_visual.Children();
    auto compVisual = CompVisualFromVisual(visual);
    if (index == 0 || containerChildren.Count() == 0) {
      containerChildren.InsertAtTop(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(IVisual *visual) noexcept override {
    auto compVisual = CompVisualFromVisual(visual);
    auto containerChildren = m_visual.Children();
    containerChildren.Remove(compVisual);
  }

  void Brush(IBrush *brush) noexcept override {
    m_visual.Brush(CompBrushFromBrush(brush));
  }

  void Opacity(float opacity) noexcept override {
    m_visual.Opacity(opacity);
  }

  void Scale(winrt::Windows::Foundation::Numerics::float3 const &scale) noexcept override {
    m_visual.Scale(scale);
  }

  void RotationAngle(float rotation) noexcept override {
    m_visual.RotationAngle(rotation);
  }

  void IsVisible(bool isVisible) noexcept override {
    m_visual.IsVisible(isVisible);
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept override {
    m_visual.Size(size);
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept override {
    m_visual.Offset(offset);
  }

  void Shadow(IDropShadow *shadow) noexcept override {
    m_visual.Shadow(CompShadowFromShadow(shadow));
  }

 private:
  winrt::Windows::UI::Composition::SpriteVisual m_visual;
};

struct CompScrollerVisual : public winrt::implements<CompScrollerVisual, IScrollerVisual, ICompositionVisual> {
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
      m_outer->m_scrollCallback({args.Position().x, args.Position().y});
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

  winrt::Windows::UI::Composition::Visual InnerVisual() const noexcept override {
    return m_visual;
  }

  void InsertAt(IVisual *visual, uint32_t index) noexcept override {
    auto containerChildren = m_contentVisual.Children();
    auto compVisual = CompVisualFromVisual(visual);
    if (index == 0 || containerChildren.Count() == 0) {
      containerChildren.InsertAtTop(compVisual);
      return;
    }
    auto insertAfter = containerChildren.First();
    for (uint32_t i = 1; i < index; i++)
      insertAfter.MoveNext();
    containerChildren.InsertAbove(compVisual, insertAfter.Current());
  }

  void Remove(IVisual *visual) noexcept override {
    auto compVisual = CompVisualFromVisual(visual);
    auto containerChildren = m_contentVisual.Children();
    containerChildren.Remove(compVisual);
  }

  void Brush(IBrush *brush) noexcept override {
    m_visual.Brush(CompBrushFromBrush(brush));
  }

  void Opacity(float opacity) noexcept override {
    m_visual.Opacity(opacity);
  }

  void Scale(winrt::Windows::Foundation::Numerics::float3 const &scale) noexcept override {
    m_visual.Scale(scale);
  }

  void RotationAngle(float rotation) noexcept override {
    m_visual.RotationAngle(rotation);
  }

  void IsVisible(bool isVisible) noexcept override {
    m_visual.IsVisible(isVisible);
  }

  void Size(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept override {
    m_visual.Size(size);
  }

  void Offset(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept override {
    m_visual.Offset(offset);
  }

  void Shadow(IDropShadow *shadow) noexcept override {
    m_visual.Shadow(CompShadowFromShadow(shadow));
  }

  void SetOnScrollCallback(
      std::function<void(winrt::Windows::Foundation::Numerics::float2)> &&callback) noexcept override {
    m_scrollCallback = std::move(callback);
  }

  void ContentSize(winrt::Windows::Foundation::Numerics::float2 const &size) noexcept override {
    m_contentVisual.Size(size);
    m_interactionTracker.MaxPosition({size.x, size.y, 0});
  }

  winrt::Windows::Foundation::Numerics::float3 const ScrollPosition() noexcept override {
    return m_interactionTracker.Position();
  }

  void ScrollBy(winrt::Windows::Foundation::Numerics::float3 const &offset) noexcept override {
    m_interactionTracker.TryUpdatePositionBy(offset);
  };

 private:
  std::function<void(winrt::Windows::Foundation::Numerics::float2)> m_scrollCallback{nullptr};
  winrt::Windows::UI::Composition::SpriteVisual m_visual;
  winrt::Windows::UI::Composition::SpriteVisual m_contentVisual{nullptr};
  winrt::Windows::UI::Composition::Interactions::InteractionTracker m_interactionTracker{nullptr};
  winrt::Windows::UI::Composition::Interactions::VisualInteractionSource m_visualInteractionSource{nullptr};
};

struct CompContext : winrt::implements<
                         CompContext,
                         winrt::Microsoft::ReactNative::Composition::ICompositionContext,
                         ICompositionContext> {
  CompContext(winrt::Windows::UI::Composition::Compositor const &compositor) : m_compositor(compositor) {}

  // Work around winrt ICompositionContext not being able to be an empty interface
  int64_t Handle() const noexcept {
    return 0;
  }

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

  void CreateDrawingSurface(
      winrt::Windows::Foundation::Size surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode alphaMode,
      Composition::ICompositionDrawingSurface **drawingSurfaceOut) noexcept {
    winrt::make<Composition::CompositionDrawingSurface>(
        CompositionGraphicsDevice().CreateDrawingSurface(surfaceSize, pixelFormat, alphaMode))
        .copy_to(drawingSurfaceOut);
  }

  void CreateSpriteVisual(Composition::ISpriteVisual **visualOut) noexcept {
    winrt::make<Composition::CompSpriteVisual>(m_compositor.CreateSpriteVisual()).copy_to(visualOut);
  }

  void CreateScrollerVisual(Composition::IScrollerVisual **scrollerOut) noexcept {
    winrt::make<Composition::CompScrollerVisual>(m_compositor.CreateSpriteVisual()).copy_to(scrollerOut);
  }

  void CreateDropShadow(Composition::IDropShadow **shadowOut) noexcept {
    winrt::make<Composition::CompDropShadow>(m_compositor.CreateDropShadow()).copy_to(shadowOut);
  }

  void CreateColorBrush(winrt::Windows::UI::Color color, Composition::IBrush **brushOut) noexcept {
    winrt::make<Composition::CompBrush>(m_compositor.CreateColorBrush(color)).copy_to(brushOut);
  }

  void CreateSurfaceBrush(
      Composition::ICompositionDrawingSurface *surface,
      Composition::ISurfaceBrush **surfaceBrushOut) noexcept {
    winrt::make<Composition::CompSurfaceBrush>(m_compositor, surface).copy_to(surfaceBrushOut);
  }

  void CreateCaratVisual(Composition::IVisual **visualOut) noexcept {
    auto compVisual = m_compositor.CreateSpriteVisual();
    auto carat = winrt::make<Composition::CompSpriteVisual>(compVisual);

    compVisual.Brush(m_compositor.CreateColorBrush(winrt::Windows::UI::Colors::Black()));
    compVisual.Opacity(1.0f);
    compVisual.RelativeSizeAdjustment({0.0f, 1.0f});

    // Blinking animation
    constexpr float ftCaretFadePct = 0.2385714285714f;
    constexpr float stayVisFrame = (1.0f - ftCaretFadePct) / 2.0f;
    constexpr float fadeVisFrame = ftCaretFadePct / 2.0f;

    auto opacityAnimation = m_compositor.CreateScalarKeyFrameAnimation();
    opacityAnimation.InsertKeyFrame(0.0f, 1.0f);
    opacityAnimation.InsertKeyFrame(stayVisFrame, 1.0f);
    opacityAnimation.InsertKeyFrame(stayVisFrame + fadeVisFrame, 0.0f, m_compositor.CreateLinearEasingFunction());
    opacityAnimation.InsertKeyFrame(stayVisFrame + fadeVisFrame + stayVisFrame, 0.0f);
    opacityAnimation.InsertKeyFrame(1.0f, 1.0f, m_compositor.CreateLinearEasingFunction());
    opacityAnimation.Duration(std::chrono::milliseconds{1000});
    opacityAnimation.IterationBehavior(winrt::Windows::UI::Composition::AnimationIterationBehavior::Forever);

    carat.as<Composition::IVisual>().copy_to(visualOut);
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

ICompositionVisual CompositionContextHelper::CreateVisual(
    winrt::Windows::UI::Composition::Visual const &visual) noexcept {
  return winrt::make<::Microsoft::ReactNative::Composition::CompVisual>(visual).as<ICompositionVisual>();
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
