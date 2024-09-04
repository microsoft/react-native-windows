
/*
 * This file is auto-generated from MovingLightNativeComponent spec file in flow / TypeScript.
 */
#pragma once

#include "NativeComponents.h"

namespace winrt::SampleCustomComponent::Codegen {

REACT_STRUCT(MovingLightSpec_MovingLightProps_objectProp)
struct MovingLightSpec_MovingLightProps_objectProp {
  REACT_FIELD(number)
  double number{};

  REACT_FIELD(string)
  std::string string;
};

REACT_STRUCT(MovingLightProps)
struct MovingLightProps : winrt::implements<MovingLightProps, winrt::Microsoft::ReactNative::IComponentProps> {
  MovingLightProps(winrt::Microsoft::ReactNative::ViewProps props) : m_props(props) {}

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(size)
  float size{42};

  REACT_FIELD(color)
  winrt::Microsoft::ReactNative::Color color{nullptr};

  REACT_FIELD(eventParam)
  std::optional<std::string> eventParam;

  REACT_FIELD(objectProp)
  std::optional<MovingLightSpec_MovingLightProps_objectProp> objectProp;

  winrt::Microsoft::ReactNative::ViewProps m_props;
};

REACT_STRUCT(MovingLight_OnSomething)
struct MovingLight_OnSomething {
  REACT_FIELD(value)
  std::string value;

  REACT_FIELD(target)
  int32_t target{};
};

struct MovingLightEventEmitter {
  MovingLightEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

  using OnSomething = MovingLight_OnSomething;

  void onSomething(OnSomething &value) const {
    m_eventEmitter.DispatchEvent(L"something", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

template <typename TUserData, typename TProps = MovingLightProps, typename TEventEmitter = MovingLightEventEmitter>
void RegisterMovingLightNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"MovingLight", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps(
            [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<TProps>(props); });

        RegisterUpdatePropsHandler<TUserData, TProps>(builder);
        RegisterUpdateEventEmitterHandler<TUserData, TEventEmitter>(builder);
        RegisterFinalizeUpdateHandler<TUserData>(builder);
        RegisterComponentInitializer<TUserData>(builder);
        RegisterComponentCreateVisual<TUserData>(builder);

        // Allow app to further customize the builder
        if (builderCallback) {
          builderCallback(compBuilder);
        }
      });
}

} // namespace winrt::SampleCustomComponent::Codegen
