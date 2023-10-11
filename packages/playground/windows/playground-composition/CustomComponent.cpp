#include "pch.h"

#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.ReactNative.Composition.h>

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

  void UpdateLayoutMetrics(winrt::Microsoft::ReactNative::Composition::LayoutMetrics metrics) noexcept {
    m_visual.Size({metrics.Frame.Width, metrics.Frame.Height});
  }

  winrt::Microsoft::ReactNative::Composition::IVisual CreateVisual() noexcept {
    m_visual = m_compContext.CreateSpriteVisual();
    m_visual.Brush(m_compContext.CreateColorBrush(winrt::Windows::UI::Colors::White()));

    auto compositor =
        winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper::InnerCompositor(m_compContext);

    m_spotlight = compositor.CreateSpotLight();
    m_spotlight.InnerConeAngleInDegrees(50.0f);
    m_spotlight.InnerConeColor(winrt::Windows::UI::Colors::FloralWhite());
    m_spotlight.InnerConeIntensity(5.0f);
    m_spotlight.OuterConeAngleInDegrees(0.0f);
    m_spotlight.ConstantAttenuation(1.0f);
    m_spotlight.LinearAttenuation(0.253f);
    m_spotlight.QuadraticAttenuation(0.58f);
    m_spotlight.CoordinateSpace(
        winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper::InnerVisual(m_visual));
    m_spotlight.Targets().Add(
        winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper::InnerVisual(m_visual));

    auto animation = compositor.CreateVector3KeyFrameAnimation();
    auto easeIn = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {1.0f, 1.0f});
    animation.InsertKeyFrame(0.00f, {100.0f, 100.0f, 35.0f});
    animation.InsertKeyFrame(0.25f, {300.0f, 200.0f, 75.0f}, easeIn);
    animation.InsertKeyFrame(0.50f, {050.0f, 300.0f, 15.0f}, easeIn);
    animation.InsertKeyFrame(0.75f, {300.0f, 050.0f, 75.0f}, easeIn);
    animation.InsertKeyFrame(1.00f, {100.0f, 100.0f, 35.0f}, easeIn);
    animation.Duration(std::chrono::milliseconds(4000));
    animation.IterationBehavior(winrt::Windows::UI::Composition::AnimationIterationBehavior::Forever);

    m_spotlight.StartAnimation(L"Offset", animation);

    return m_visual;
  }

  // TODO - Once we get more complete native eventing we can move spotlight based on pointer position
  void OnPointerMove() noexcept {
    // m_spotlight.Offset({(float)x, (float)y, 15.0f});

    // m_propSet.InsertVector2(L"Position", {x, y});
    //  TODO expose coordinate translation methods
    //  TODO convert x/y into local coordinates
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
        });
  }

 private:
  winrt::Windows::UI::Composition::SpotLight m_spotlight{nullptr};

  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICompositionContext m_compContext;
};

void RegisterCustomComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    CustomComponent::RegisterViewComponent(packageBuilder);
}
