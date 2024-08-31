// This entire file should be codegen-able

#include "NativeComponents.h"

namespace winrt::SampleCustomComponent::MovingLightCodegen {

// Should be codegen'd
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
  std::string eventParam;

  winrt::Microsoft::ReactNative::ViewProps m_props;
};

// Should be codegen'd
REACT_STRUCT(OnSomething)
struct OnSomething {
  REACT_FIELD(value)
  std::string value;
};

// Should be codegen'd
struct MovingLightEventEmitter {
  MovingLightEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

  void onSomething(OnSomething &value) const {
    m_eventEmitter.DispatchEvent(L"Something", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

// codegen
template <typename TUserData, typename TProps = MovingLightProps, typename TEventEmitter = MovingLightEventEmitter>
void ConfigureMovingLightNativeComponent(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) {
  auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

  // Always
  builder.SetCreateProps(
      [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<TProps>(props); });

  RegisterUpdatePropsHandler<TUserData, TProps>(builder);
  RegisterUpdateEventEmitterHandler<TUserData, TEventEmitter>(builder);
  RegisterFinalizeUpdateHandler<TUserData>(builder);
  RegisterComponentInitializer<TUserData>(builder);
  RegisterComponentCreateVisual<TUserData>(builder);
}

} // namespace MovingLightCodegen
