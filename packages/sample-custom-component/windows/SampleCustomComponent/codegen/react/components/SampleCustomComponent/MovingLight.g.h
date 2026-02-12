
/*
 * This file is auto-generated from MovingLightNativeComponent spec file in flow / TypeScript.
 */
// clang-format off
#pragma once

#include <NativeModules.h>

#ifdef RNW_NEW_ARCH
#include <JSValueComposition.h>

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.UI.Composition.h>
#endif // #ifdef RNW_NEW_ARCH

#ifdef RNW_NEW_ARCH

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
  MovingLightProps(winrt::Microsoft::ReactNative::ViewProps props, const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom)
    : ViewProps(props)
  {
     if (cloneFrom) {
       auto cloneFromProps = cloneFrom.as<MovingLightProps>();
       size = cloneFromProps->size;
       color = cloneFromProps->color;
       testMixed = cloneFromProps->testMixed.Copy();
       eventParam = cloneFromProps->eventParam;
       objectProp = cloneFromProps->objectProp;  
     }
  }

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(size)
  float size{42};

  REACT_FIELD(color)
  winrt::Microsoft::ReactNative::Color color{nullptr};

  REACT_FIELD(testMixed)
  winrt::Microsoft::ReactNative::JSValue testMixed{nullptr};

  REACT_FIELD(eventParam)
  std::optional<std::string> eventParam;

  REACT_FIELD(objectProp)
  std::optional<MovingLightSpec_MovingLightProps_objectProp> objectProp;

  const winrt::Microsoft::ReactNative::ViewProps ViewProps;
};

REACT_STRUCT(MovingLightSpec_onEventWithMultipleAliasTypes6)
struct MovingLightSpec_onEventWithMultipleAliasTypes6 {
  REACT_FIELD(x)
  double x{};

  REACT_FIELD(y)
  double y{};
};

REACT_STRUCT(MovingLightSpec_onEventWithMultipleAliasTypes5)
struct MovingLightSpec_onEventWithMultipleAliasTypes5 {
  REACT_FIELD(width)
  double width{};

  REACT_FIELD(height)
  double height{};
};

REACT_STRUCT(MovingLightSpec_onEventWithMultipleAliasTypes4)
struct MovingLightSpec_onEventWithMultipleAliasTypes4 {
  REACT_FIELD(width)
  double width{};

  REACT_FIELD(height)
  double height{};
};

REACT_STRUCT(MovingLightSpec_onEventWithMultipleAliasTypes3)
struct MovingLightSpec_onEventWithMultipleAliasTypes3 {
  REACT_FIELD(x)
  double x{};

  REACT_FIELD(y)
  double y{};
};

REACT_STRUCT(MovingLightSpec_onEventWithMultipleAliasTypes2)
struct MovingLightSpec_onEventWithMultipleAliasTypes2 {
  REACT_FIELD(top)
  double top{};

  REACT_FIELD(bottom)
  double bottom{};

  REACT_FIELD(left)
  double left{};

  REACT_FIELD(right)
  double right{};
};

REACT_STRUCT(MovingLightSpec_onEventWithInlineTypes6)
struct MovingLightSpec_onEventWithInlineTypes6 {
  REACT_FIELD(x)
  double x{};

  REACT_FIELD(y)
  double y{};
};

REACT_STRUCT(MovingLightSpec_onEventWithInlineTypes5)
struct MovingLightSpec_onEventWithInlineTypes5 {
  REACT_FIELD(width)
  double width{};

  REACT_FIELD(height)
  double height{};
};

REACT_STRUCT(MovingLightSpec_onEventWithInlineTypes4)
struct MovingLightSpec_onEventWithInlineTypes4 {
  REACT_FIELD(width)
  double width{};

  REACT_FIELD(height)
  double height{};
};

REACT_STRUCT(MovingLightSpec_onEventWithInlineTypes3)
struct MovingLightSpec_onEventWithInlineTypes3 {
  REACT_FIELD(x)
  double x{};

  REACT_FIELD(y)
  double y{};
};

REACT_STRUCT(MovingLightSpec_onEventWithInlineTypes2)
struct MovingLightSpec_onEventWithInlineTypes2 {
  REACT_FIELD(top)
  double top{};

  REACT_FIELD(bottom)
  double bottom{};

  REACT_FIELD(left)
  double left{};

  REACT_FIELD(right)
  double right{};
};

REACT_STRUCT(MovingLightSpec_onEventWithMultipleAliasTypes)
struct MovingLightSpec_onEventWithMultipleAliasTypes {
  REACT_FIELD(target)
  int32_t target{};

  REACT_FIELD(contentInset)
  MovingLightSpec_onEventWithMultipleAliasTypes2 contentInset;

  REACT_FIELD(contentOffset)
  MovingLightSpec_onEventWithMultipleAliasTypes3 contentOffset;

  REACT_FIELD(contentSize)
  MovingLightSpec_onEventWithMultipleAliasTypes4 contentSize;

  REACT_FIELD(layoutMeasurement)
  MovingLightSpec_onEventWithMultipleAliasTypes5 layoutMeasurement;

  REACT_FIELD(velocity)
  MovingLightSpec_onEventWithMultipleAliasTypes6 velocity;

  REACT_FIELD(isUserTriggered)
  bool isUserTriggered{};
};

REACT_STRUCT(MovingLightSpec_onEventWithInlineTypes)
struct MovingLightSpec_onEventWithInlineTypes {
  REACT_FIELD(target)
  int32_t target{};

  REACT_FIELD(contentInset)
  MovingLightSpec_onEventWithInlineTypes2 contentInset;

  REACT_FIELD(contentOffset)
  MovingLightSpec_onEventWithInlineTypes3 contentOffset;

  REACT_FIELD(contentSize)
  MovingLightSpec_onEventWithInlineTypes4 contentSize;

  REACT_FIELD(layoutMeasurement)
  MovingLightSpec_onEventWithInlineTypes5 layoutMeasurement;

  REACT_FIELD(velocity)
  MovingLightSpec_onEventWithInlineTypes6 velocity;

  REACT_FIELD(isUserTriggered)
  bool isUserTriggered{};
};

REACT_STRUCT(MovingLightSpec_onTestObjectEvent)
struct MovingLightSpec_onTestObjectEvent {
  REACT_FIELD(target)
  int32_t target{};

  REACT_FIELD(testObject)
  winrt::Microsoft::ReactNative::JSValue testObject{nullptr};
};

REACT_STRUCT(MovingLightSpec_onSomething)
struct MovingLightSpec_onSomething {
  REACT_FIELD(value)
  std::string value;

  REACT_FIELD(target)
  int32_t target{};
};

struct MovingLightEventEmitter {
  MovingLightEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

  using OnSomething = MovingLightSpec_onSomething;
  using OnTestObjectEvent = MovingLightSpec_onTestObjectEvent;
  using OnEventWithInlineTypes = MovingLightSpec_onEventWithInlineTypes;
  using OnEventWithMultipleAliasTypes = MovingLightSpec_onEventWithMultipleAliasTypes;

  void onSomething(OnSomething &value) const {
    m_eventEmitter.DispatchEvent(L"something", [&value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

  void onTestObjectEvent(OnTestObjectEvent &value) const {
    m_eventEmitter.DispatchEvent(L"testObjectEvent", [&value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

  void onEventWithInlineTypes(OnEventWithInlineTypes &value) const {
    m_eventEmitter.DispatchEvent(L"eventWithInlineTypes", [&value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

  void onEventWithMultipleAliasTypes(OnEventWithMultipleAliasTypes &value) const {
    m_eventEmitter.DispatchEvent(L"eventWithMultipleAliasTypes", [&value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

template<typename TUserData>
struct BaseMovingLight {

  virtual void UpdateProps(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::com_ptr<MovingLightProps> &newProps,
    const winrt::com_ptr<MovingLightProps> &/*oldProps*/) noexcept {
    m_props = newProps;
  }

  // UpdateLayoutMetrics will only be called if this method is overridden
  virtual void UpdateLayoutMetrics(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::Microsoft::ReactNative::LayoutMetrics &/*newLayoutMetrics*/,
    const winrt::Microsoft::ReactNative::LayoutMetrics &/*oldLayoutMetrics*/) noexcept {
  }

  // UpdateState will only be called if this method is overridden
  virtual void UpdateState(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::Microsoft::ReactNative::IComponentState &/*newState*/) noexcept {
  }

  virtual void UpdateEventEmitter(const std::shared_ptr<MovingLightEventEmitter> &eventEmitter) noexcept {
    m_eventEmitter = eventEmitter;
  }

  // MountChildComponentView will only be called if this method is overridden
  virtual void MountChildComponentView(const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
           const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &/*args*/) noexcept {
  }

  // UnmountChildComponentView will only be called if this method is overridden
  virtual void UnmountChildComponentView(const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
           const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &/*args*/) noexcept {
  }

  // Initialize will only be called if this method is overridden
  virtual void Initialize(const winrt::Microsoft::ReactNative::ComponentView &/*view*/) noexcept {
  }

  // CreateVisual will only be called if this method is overridden
  virtual winrt::Microsoft::UI::Composition::Visual CreateVisual(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    return view.as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Compositor().CreateSpriteVisual();
  }

  // FinalizeUpdate will only be called if this method is overridden
  virtual void FinalizeUpdate(const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
                                        winrt::Microsoft::ReactNative::ComponentViewUpdateMask /*mask*/) noexcept {
  }

  // CreateAutomationPeer will only be called if this method is overridden
  virtual winrt::Windows::Foundation::IInspectable CreateAutomationPeer(const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
                                        const winrt::Microsoft::ReactNative::CreateAutomationPeerArgs& /*args*/) noexcept {
    return nullptr;
  }

  // You must provide an implementation of this method to handle the "setLightOn" command
  virtual void HandleSetLightOnCommand(bool value) noexcept = 0;

  void HandleCommand(const winrt::Microsoft::ReactNative::ComponentView &view, const winrt::Microsoft::ReactNative::HandleCommandArgs& args) noexcept {
    auto userData = view.UserData().as<TUserData>();
    auto commandName = args.CommandName();
    if (commandName == L"setLightOn") {
      bool value;
      winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), value);
      userData->HandleSetLightOnCommand(value);
      return;
    }
  }

  const std::shared_ptr<MovingLightEventEmitter>& EventEmitter() const { return m_eventEmitter; }
  const winrt::com_ptr<MovingLightProps>& Props() const { return m_props; }

private:
  winrt::com_ptr<MovingLightProps> m_props;
  std::shared_ptr<MovingLightEventEmitter> m_eventEmitter;
};

template <typename TUserData>
void RegisterMovingLightNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"MovingLight", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props,
                              const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom) noexcept {
            return winrt::make<MovingLightProps>(props, cloneFrom); 
        });

        builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateProps(view, newProps ? newProps.as<MovingLightProps>() : nullptr, oldProps ? oldProps.as<MovingLightProps>() : nullptr);
        });

        compBuilder.SetUpdateLayoutMetricsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::LayoutMetrics &newLayoutMetrics,
                                      const winrt::Microsoft::ReactNative::LayoutMetrics &oldLayoutMetrics) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateLayoutMetrics(view, newLayoutMetrics, oldLayoutMetrics);
        });

        builder.SetUpdateEventEmitterHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter) noexcept {
          auto userData = view.UserData().as<TUserData>();
          userData->UpdateEventEmitter(std::make_shared<MovingLightEventEmitter>(eventEmitter));
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::FinalizeUpdate != &BaseMovingLight<TUserData>::FinalizeUpdate) {
            builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->FinalizeUpdate(view, mask);
          });
        } 

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UpdateState != &BaseMovingLight<TUserData>::UpdateState) {
          builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateState(view, newState);
          });
        }

        builder.SetCustomCommandHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                          const winrt::Microsoft::ReactNative::HandleCommandArgs& args) noexcept {
          auto userData = view.UserData().as<TUserData>();
          userData->HandleCommand(view, args);
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::MountChildComponentView != &BaseMovingLight<TUserData>::MountChildComponentView) {
          builder.SetMountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->MountChildComponentView(view, args);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UnmountChildComponentView != &BaseMovingLight<TUserData>::UnmountChildComponentView) {
          builder.SetUnmountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->UnmountChildComponentView(view, args);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::CreateAutomationPeer != &BaseMovingLight<TUserData>::CreateAutomationPeer) {
            builder.SetCreateAutomationPeerHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::CreateAutomationPeerArgs& args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->CreateAutomationPeer(view, args);
          });
        } 

        compBuilder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
          auto userData = winrt::make_self<TUserData>();
          if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::Initialize != &BaseMovingLight<TUserData>::Initialize) {
            userData->Initialize(view);
          }
          view.UserData(*userData);
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::CreateVisual != &BaseMovingLight<TUserData>::CreateVisual) {
          compBuilder.SetCreateVisualHandler([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->CreateVisual(view);
          });
        }

        // Allow app to further customize the builder
        if (builderCallback) {
          builderCallback(compBuilder);
        }
      });
}

} // namespace winrt::SampleCustomComponent::Codegen

#endif // #ifdef RNW_NEW_ARCH
