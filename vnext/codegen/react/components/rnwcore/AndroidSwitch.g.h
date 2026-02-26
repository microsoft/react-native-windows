
/*
 * This file is auto-generated from AndroidSwitchNativeComponent spec file in flow / TypeScript.
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

namespace Microsoft::ReactNativeSpecs {

REACT_STRUCT(AndroidSwitchProps)
struct AndroidSwitchProps : winrt::implements<AndroidSwitchProps, winrt::Microsoft::ReactNative::IComponentProps> {
  AndroidSwitchProps(winrt::Microsoft::ReactNative::ViewProps props, const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom)
    : ViewProps(props)
  {
     if (cloneFrom) {
       auto cloneFromProps = cloneFrom.as<AndroidSwitchProps>();
       disabled = cloneFromProps->disabled;
       enabled = cloneFromProps->enabled;
       thumbColor = cloneFromProps->thumbColor;
       trackColorForFalse = cloneFromProps->trackColorForFalse;
       trackColorForTrue = cloneFromProps->trackColorForTrue;
       value = cloneFromProps->value;
       on = cloneFromProps->on;
       thumbTintColor = cloneFromProps->thumbTintColor;
       trackTintColor = cloneFromProps->trackTintColor;  
     }
  }

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(disabled)
  std::optional<bool> disabled{};

  REACT_FIELD(enabled)
  bool enabled{true};

  REACT_FIELD(thumbColor)
  winrt::Microsoft::ReactNative::Color thumbColor{nullptr};

  REACT_FIELD(trackColorForFalse)
  winrt::Microsoft::ReactNative::Color trackColorForFalse{nullptr};

  REACT_FIELD(trackColorForTrue)
  winrt::Microsoft::ReactNative::Color trackColorForTrue{nullptr};

  REACT_FIELD(value)
  std::optional<bool> value{};

  REACT_FIELD(on)
  std::optional<bool> on{};

  REACT_FIELD(thumbTintColor)
  winrt::Microsoft::ReactNative::Color thumbTintColor{nullptr};

  REACT_FIELD(trackTintColor)
  winrt::Microsoft::ReactNative::Color trackTintColor{nullptr};

  const winrt::Microsoft::ReactNative::ViewProps ViewProps;
};

REACT_STRUCT(AndroidSwitchSpec_onChange)
struct AndroidSwitchSpec_onChange {
  REACT_FIELD(value)
  bool value{};

  REACT_FIELD(target)
  int32_t target{};
};

struct AndroidSwitchEventEmitter {
  AndroidSwitchEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

  using OnChange = AndroidSwitchSpec_onChange;

  void onChange(OnChange &&value) const {
    m_eventEmitter.DispatchEvent(L"change", [value = std::move(value)](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

template<typename TUserData>
struct BaseAndroidSwitch {

  virtual void UpdateProps(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::com_ptr<AndroidSwitchProps> &newProps,
    const winrt::com_ptr<AndroidSwitchProps> &/*oldProps*/) noexcept {
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

  virtual void UpdateEventEmitter(const std::shared_ptr<AndroidSwitchEventEmitter> &eventEmitter) noexcept {
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

  // You must provide an implementation of this method to handle the "setNativeValue" command
  virtual void HandleSetNativeValueCommand(bool value) noexcept = 0;

  void HandleCommand(const winrt::Microsoft::ReactNative::ComponentView &view, const winrt::Microsoft::ReactNative::HandleCommandArgs& args) noexcept {
    auto userData = view.UserData().as<TUserData>();
    auto commandName = args.CommandName();
    if (commandName == L"setNativeValue") {
      bool value;
      winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), value);
      userData->HandleSetNativeValueCommand(value);
      return;
    }
  }

  const std::shared_ptr<AndroidSwitchEventEmitter>& EventEmitter() const { return m_eventEmitter; }
  const winrt::com_ptr<AndroidSwitchProps>& Props() const { return m_props; }

private:
  winrt::com_ptr<AndroidSwitchProps> m_props;
  std::shared_ptr<AndroidSwitchEventEmitter> m_eventEmitter;
};

template <typename TUserData>
void RegisterAndroidSwitchNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"AndroidSwitch", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props,
                              const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom) noexcept {
            return winrt::make<AndroidSwitchProps>(props, cloneFrom); 
        });

        builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateProps(view, newProps ? newProps.as<AndroidSwitchProps>() : nullptr, oldProps ? oldProps.as<AndroidSwitchProps>() : nullptr);
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
          userData->UpdateEventEmitter(std::make_shared<AndroidSwitchEventEmitter>(eventEmitter));
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::FinalizeUpdate != &BaseAndroidSwitch<TUserData>::FinalizeUpdate) {
            builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->FinalizeUpdate(view, mask);
          });
        } 

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UpdateState != &BaseAndroidSwitch<TUserData>::UpdateState) {
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

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::MountChildComponentView != &BaseAndroidSwitch<TUserData>::MountChildComponentView) {
          builder.SetMountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->MountChildComponentView(view, args);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UnmountChildComponentView != &BaseAndroidSwitch<TUserData>::UnmountChildComponentView) {
          builder.SetUnmountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->UnmountChildComponentView(view, args);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::CreateAutomationPeer != &BaseAndroidSwitch<TUserData>::CreateAutomationPeer) {
            builder.SetCreateAutomationPeerHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::CreateAutomationPeerArgs& args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->CreateAutomationPeer(view, args);
          });
        } 

        compBuilder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
          auto userData = winrt::make_self<TUserData>();
          if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::Initialize != &BaseAndroidSwitch<TUserData>::Initialize) {
            userData->Initialize(view);
          }
          view.UserData(*userData);
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::CreateVisual != &BaseAndroidSwitch<TUserData>::CreateVisual) {
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

} // namespace Microsoft::ReactNativeSpecs

#endif // #ifdef RNW_NEW_ARCH
