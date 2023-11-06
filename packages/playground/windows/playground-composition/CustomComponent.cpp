#include "pch.h"

#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.UI.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Composition.h>
#endif

/*
 * Custom Properties can be passed from JS to this native component
 * This struct will eventually be codegen'd from the JS spec file
 */
struct CustomProps : winrt::implements<CustomProps, winrt::Microsoft::ReactNative::IComponentProps> {
  CustomProps(winrt::Microsoft::ReactNative::ViewProps props) : m_props(props) {}

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    if (propName == L"label") {
      if (!value) {
        label.clear();
      } else {
        label = winrt::to_string(value.GetString());
      }
    }
  }

  std::string label;
  winrt::Microsoft::ReactNative::ViewProps m_props;
};

struct CustomComponent : winrt::implements<CustomComponent, winrt::IInspectable> {
  CustomComponent(
      winrt::Microsoft::ReactNative::IReactContext reactContext,
      winrt::Microsoft::ReactNative::Composition::ICompositionContext compContext)
      : m_compContext(compContext) {}

  void UpdateProps(winrt::Microsoft::ReactNative::IComponentProps props) noexcept {
    auto customProps = props.as<CustomProps>();
  }

  void UpdateLayoutMetrics(winrt::Microsoft::ReactNative::Composition::LayoutMetrics layoutMetrics) noexcept {
    m_layoutMetrics = layoutMetrics;
    m_visual.Size(
        {m_layoutMetrics.Frame.Width * m_layoutMetrics.PointScaleFactor,
         m_layoutMetrics.Frame.Height * m_layoutMetrics.PointScaleFactor});
  }

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() noexcept {
    m_visual = m_compContext.CreateSpriteVisual();
    m_visual.Brush(m_compContext.CreateColorBrush(winrt::Windows::UI::Colors::White()));

#ifdef USE_WINUI3
    auto compositor =
        winrt::Microsoft::ReactNative::Composition::MicrosoftCompositionContextHelper::InnerCompositor(m_compContext);

    if (compositor) {
      m_spotlight = compositor.CreateSpotLight();
      m_spotlight.InnerConeAngleInDegrees(50.0f);
      m_spotlight.InnerConeColor(winrt::Windows::UI::Colors::FloralWhite());
      m_spotlight.InnerConeIntensity(5.0f);
      m_spotlight.OuterConeAngleInDegrees(0.0f);
      m_spotlight.ConstantAttenuation(1.0f);
      m_spotlight.LinearAttenuation(0.253f);
      m_spotlight.QuadraticAttenuation(0.58f);
      m_spotlight.CoordinateSpace(
          winrt::Microsoft::ReactNative::Composition::MicrosoftCompositionContextHelper::InnerVisual(m_visual));
      m_spotlight.Targets().Add(
          winrt::Microsoft::ReactNative::Composition::MicrosoftCompositionContextHelper::InnerVisual(m_visual));

      auto implicitAnimations = compositor.CreateImplicitAnimationCollection();
      implicitAnimations.Insert(L"Offset", CreateAnimation(compositor));
      m_spotlight.ImplicitAnimations(implicitAnimations);
    }
#endif

    return m_visual;
  }

#ifdef USE_WINUI3
  winrt::Microsoft::UI::Composition::Vector3KeyFrameAnimation CreateAnimation(
      winrt::Microsoft::UI::Composition::Compositor compositor) {
    auto animation = compositor.CreateVector3KeyFrameAnimation();
    animation.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
    animation.InsertExpressionKeyFrame(
        1.0f, L"this.FinalValue", compositor.CreateCubicBezierEasingFunction({.38f, .29f}, {.64f, 1.52f}));
    animation.Target(L"Offset");
    animation.Duration(std::chrono::milliseconds(250));
    return animation;
  }
#endif

  void PointerMoved(const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
    // Ideally we'd get the coords based on this view's tag, but that is NYI - so we'll just mod the coords to keep them
    // in view for now
    auto position = args.GetCurrentPoint(-1).Position();
#ifdef USE_WINUI3
    m_spotlight.Offset(
        {std::fmodf(
             position.X * m_layoutMetrics.PointScaleFactor,
             m_layoutMetrics.Frame.Width * m_layoutMetrics.PointScaleFactor),
         std::fmodf(
             position.Y * m_layoutMetrics.PointScaleFactor,
             m_layoutMetrics.Frame.Height * m_layoutMetrics.PointScaleFactor),
         std::fmodf((position.X + position.Y) / 40, 50) + 15.0f});
#endif
  }

  void PointerEntered(const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
    m_visual.Brush(m_compContext.CreateColorBrush(winrt::Windows::UI::Colors::Red()));
  }

  void PointerExited(const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
    m_visual.Brush(m_compContext.CreateColorBrush(winrt::Windows::UI::Colors::White()));
#ifdef USE_WINUI3
    m_spotlight.Offset(
        {m_layoutMetrics.Frame.Width / 2.0f * m_layoutMetrics.PointScaleFactor,
         m_layoutMetrics.Frame.Height / 2 * m_layoutMetrics.PointScaleFactor,
         75.0f});
#endif
  }

  static void RegisterViewComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
    packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
        L"MyCustomComponent", [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
          builder.SetCreateProps(
              [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<CustomProps>(props); });
          auto compBuilder =
              builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();
          compBuilder.SetCreateView(
              [](winrt::Microsoft::ReactNative::IReactContext reactContext,
                 winrt::Microsoft::ReactNative::Composition::ICompositionContext context) noexcept {
                return winrt::make<CustomComponent>(reactContext, context);
              });
          compBuilder.SetPropsUpdater([](winrt::Windows::Foundation::IInspectable handle,
                                         winrt::Microsoft::ReactNative::IComponentProps props) noexcept {
            handle.as<CustomComponent>()->UpdateProps(props);
          });
          compBuilder.SetLayoutMetricsUpdater(
              [](winrt::Windows::Foundation::IInspectable handle,
                 winrt::Microsoft::ReactNative::Composition::LayoutMetrics metrics) noexcept {
                handle.as<CustomComponent>()->UpdateLayoutMetrics(metrics);
              });
          compBuilder.SetVisualCreator([](winrt::Windows::Foundation::IInspectable handle) noexcept {
            return handle.as<CustomComponent>()->CreateVisual();
          });
          compBuilder.SetPointerMovedHandler(
              [](winrt::Windows::Foundation::IInspectable handle,
                 const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
                return handle.as<CustomComponent>()->PointerMoved(args);
              });
          compBuilder.SetPointerEnteredHandler(
              [](winrt::Windows::Foundation::IInspectable handle,
                 const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
                return handle.as<CustomComponent>()->PointerEntered(args);
              });
          compBuilder.SetPointerExitedHandler(
              [](winrt::Windows::Foundation::IInspectable handle,
                 const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
                return handle.as<CustomComponent>()->PointerExited(args);
              });
        });
  }

 private:
#ifdef USE_WINUI3
  winrt::Microsoft::UI::Composition::SpotLight m_spotlight{nullptr};
#endif

  winrt::Microsoft::ReactNative::Composition::LayoutMetrics m_layoutMetrics;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICompositionContext m_compContext;
};

void RegisterCustomComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  CustomComponent::RegisterViewComponent(packageBuilder);
}
