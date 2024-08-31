#include "pch.h"

#include "SampleCustomComponent.h"

#include "SampleCustomComponent.g.h"

#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Windows.UI.h>

namespace winrt::SampleCustomComponent {

struct MovingLight : winrt::implements<MovingLight, winrt::IInspectable> {
  REACT_COMPONENT_UPDATE_PROPS(UpdateProps)
  void UpdateProps(
      const winrt::Microsoft::ReactNative::ComponentView &sender,
      const winrt::com_ptr<MovingLightCodegen::MovingLightProps> &newProps,
      const winrt::com_ptr<MovingLightCodegen::MovingLightProps> &oldProps) noexcept {
    auto view = sender.as<winrt::Microsoft::ReactNative::Composition::ViewComponentView>();
    if (!oldProps || oldProps->color != newProps->color) {
      m_spotlight.InnerConeColor(newProps->color.AsWindowsColor(view.Theme()));
    } else {
      m_spotlight.InnerConeColor(winrt::Windows::UI::Colors::FloralWhite());
    }

    if (!oldProps || oldProps->size != newProps->size) {
      m_spotlight.InnerConeAngleInDegrees(newProps->size);
    }

    if (!oldProps || oldProps->eventParam != newProps->eventParam) {
      m_eventParam = newProps->eventParam;
    }
  }

  REACT_COMPONENT_FINALIZE_UPDATES(FinalizeUpdates)
  void FinalizeUpdates(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      winrt::Microsoft::ReactNative::ComponentViewUpdateMask /*mask*/) noexcept {
    // TODO
  }

  REACT_COMPONENT_INITIALIZE(Initialize)
  void Initialize(const winrt::Microsoft::ReactNative::ComponentView & sender) noexcept {
    auto view = sender.as<winrt::Microsoft::ReactNative::Composition::ViewComponentView>();
    view.PointerPressed([wkThis = get_weak()](
                            const winrt::IInspectable & /*sender*/,
                            const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs & /*args*/) {
      if (auto strongThis = wkThis.get()) {
        if (strongThis->m_eventEmitter) {
          winrt::SampleCustomComponent::MovingLightCodegen::OnSomething eventArgs;
          eventArgs.value = strongThis->m_eventParam;
          strongThis->m_eventEmitter->onSomething(eventArgs);
        }
      }
    });
  }

  REACT_COMPONENT_CREATE_VISUAL(CreateVisual)
  winrt::Microsoft::UI::Composition::Visual CreateVisual(
      const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    auto compositor = view.as<winrt::Microsoft::ReactNative::Composition::ViewComponentView>().Compositor();
    m_visual = compositor.CreateSpriteVisual();
    m_visual.Brush(compositor.CreateColorBrush(winrt::Windows::UI::Colors::White()));

    m_spotlight = compositor.CreateSpotLight();
    m_spotlight.InnerConeAngleInDegrees(50.0f);
    m_spotlight.InnerConeIntensity(5.0f);
    m_spotlight.OuterConeAngleInDegrees(0.0f);
    m_spotlight.ConstantAttenuation(1.0f);
    m_spotlight.LinearAttenuation(0.253f);
    m_spotlight.QuadraticAttenuation(0.58f);
    m_spotlight.CoordinateSpace(m_visual);
    m_spotlight.Targets().Add(m_visual);

    auto animation = compositor.CreateVector3KeyFrameAnimation();
    auto easeIn = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {1.0f, 1.0f});
    animation.InsertKeyFrame(0.00f, {100.0f, 100.0f, 35.0f});
    animation.InsertKeyFrame(0.25f, {300.0f, 200.0f, 75.0f}, easeIn);
    animation.InsertKeyFrame(0.50f, {050.0f, 300.0f, 15.0f}, easeIn);
    animation.InsertKeyFrame(0.75f, {300.0f, 050.0f, 75.0f}, easeIn);
    animation.InsertKeyFrame(1.00f, {100.0f, 100.0f, 35.0f}, easeIn);
    animation.Duration(std::chrono::milliseconds(4000));
    animation.IterationBehavior(winrt::Microsoft::UI::Composition::AnimationIterationBehavior::Forever);

    m_spotlight.StartAnimation(L"Offset", animation);

    return m_visual;
  }

  REACT_COMPONENT_UPDATE_EVENTEMITTER(UpdateEventEmitter)
  void UpdateEventEmitter(const std::shared_ptr<MovingLightCodegen::MovingLightEventEmitter> &eventEmitter) noexcept
  {
    m_eventEmitter = eventEmitter;
  }

private:
  std::string m_eventParam;
  std::shared_ptr<MovingLightCodegen::MovingLightEventEmitter> m_eventEmitter{nullptr};
  winrt::Microsoft::UI::Composition::SpriteVisual m_visual{nullptr};
  winrt::Microsoft::UI::Composition::SpotLight m_spotlight{nullptr};

};

  void RegisterMovingLightNativeComponent(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"MovingLight",
      [](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
          MovingLightCodegen::ConfigureMovingLightNativeComponent<MovingLight>(builder); 
      });


    
  }


} // namespace winrt::SampleCustomComponent
