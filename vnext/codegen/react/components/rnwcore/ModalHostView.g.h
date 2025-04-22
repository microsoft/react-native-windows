
/*
 * This file is auto-generated from ModalHostViewNativeComponent spec file in flow / TypeScript.
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

REACT_STRUCT(ModalHostViewProps)
struct ModalHostViewProps : winrt::implements<ModalHostViewProps, winrt::Microsoft::ReactNative::IComponentProps> {
  ModalHostViewProps(winrt::Microsoft::ReactNative::ViewProps props, const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom)
    : ViewProps(props)
  {
     if (cloneFrom) {
       auto cloneFromProps = cloneFrom.as<ModalHostViewProps>();
       animationType = cloneFromProps->animationType;
       presentationStyle = cloneFromProps->presentationStyle;
       transparent = cloneFromProps->transparent;
       statusBarTranslucent = cloneFromProps->statusBarTranslucent;
       navigationBarTranslucent = cloneFromProps->navigationBarTranslucent;
       hardwareAccelerated = cloneFromProps->hardwareAccelerated;
       visible = cloneFromProps->visible;
       animated = cloneFromProps->animated;
       supportedOrientations = cloneFromProps->supportedOrientations;
       identifier = cloneFromProps->identifier;  
     }
  }

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(animationType)
  std::optional<std::string> animationType;

  REACT_FIELD(presentationStyle)
  std::optional<std::string> presentationStyle;

  REACT_FIELD(transparent)
  std::optional<bool> transparent{};

  REACT_FIELD(statusBarTranslucent)
  std::optional<bool> statusBarTranslucent{};

  REACT_FIELD(navigationBarTranslucent)
  std::optional<bool> navigationBarTranslucent{};

  REACT_FIELD(hardwareAccelerated)
  std::optional<bool> hardwareAccelerated{};

  REACT_FIELD(visible)
  std::optional<bool> visible{};

  REACT_FIELD(animated)
  std::optional<bool> animated{};

  REACT_FIELD(supportedOrientations)
  std::optional<std::vector<std::string>> supportedOrientations;

  REACT_FIELD(identifier)
  std::optional<int32_t> identifier{};

  const winrt::Microsoft::ReactNative::ViewProps ViewProps;
};

REACT_STRUCT(ModalHostView_OnRequestClose)
struct ModalHostView_OnRequestClose {
};

REACT_STRUCT(ModalHostView_OnShow)
struct ModalHostView_OnShow {
};

REACT_STRUCT(ModalHostView_OnDismiss)
struct ModalHostView_OnDismiss {
};

REACT_STRUCT(ModalHostView_OnOrientationChange)
struct ModalHostView_OnOrientationChange {
  REACT_FIELD(orientation)
  std::string orientation;
};

struct ModalHostViewEventEmitter {
  ModalHostViewEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

  using OnRequestClose = ModalHostView_OnRequestClose;
  using OnShow = ModalHostView_OnShow;
  using OnDismiss = ModalHostView_OnDismiss;
  using OnOrientationChange = ModalHostView_OnOrientationChange;

  void onRequestClose(OnRequestClose &value) const {
    m_eventEmitter.DispatchEvent(L"requestClose", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

  void onShow(OnShow &value) const {
    m_eventEmitter.DispatchEvent(L"show", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

  void onDismiss(OnDismiss &value) const {
    m_eventEmitter.DispatchEvent(L"dismiss", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

  void onOrientationChange(OnOrientationChange &value) const {
    m_eventEmitter.DispatchEvent(L"orientationChange", [value](const winrt::Microsoft::ReactNative::IJSValueWriter writer) {
      winrt::Microsoft::ReactNative::WriteValue(writer, value);
    });
  }

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

template<typename TUserData>
struct BaseModalHostView {

  virtual void UpdateProps(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::com_ptr<ModalHostViewProps> &newProps,
    const winrt::com_ptr<ModalHostViewProps> &/*oldProps*/) noexcept {
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

  virtual void UpdateEventEmitter(const std::shared_ptr<ModalHostViewEventEmitter> &eventEmitter) noexcept {
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

  

  const std::shared_ptr<ModalHostViewEventEmitter>& EventEmitter() const { return m_eventEmitter; }
  const winrt::com_ptr<ModalHostViewProps>& Props() const { return m_props; }

private:
  winrt::com_ptr<ModalHostViewProps> m_props;
  std::shared_ptr<ModalHostViewEventEmitter> m_eventEmitter;
};

template <typename TUserData>
void RegisterModalHostViewNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"ModalHostView", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props,
                              const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom) noexcept {
            return winrt::make<ModalHostViewProps>(props, cloneFrom); 
        });

        builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateProps(view, newProps ? newProps.as<ModalHostViewProps>() : nullptr, oldProps ? oldProps.as<ModalHostViewProps>() : nullptr);
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
          userData->UpdateEventEmitter(std::make_shared<ModalHostViewEventEmitter>(eventEmitter));
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::FinalizeUpdate != &BaseModalHostView<TUserData>::FinalizeUpdate) {
            builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->FinalizeUpdate(view, mask);
          });
        } 

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UpdateState != &BaseModalHostView<TUserData>::UpdateState) {
          builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateState(view, newState);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::MountChildComponentView != &BaseModalHostView<TUserData>::MountChildComponentView) {
          builder.SetMountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->MountChildComponentView(view, args);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UnmountChildComponentView != &BaseModalHostView<TUserData>::UnmountChildComponentView) {
          builder.SetUnmountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->UnmountChildComponentView(view, args);
          });
        }

        compBuilder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
          auto userData = winrt::make_self<TUserData>();
          if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::Initialize != &BaseModalHostView<TUserData>::Initialize) {
            userData->Initialize(view);
          }
          view.UserData(*userData);
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::CreateVisual != &BaseModalHostView<TUserData>::CreateVisual) {
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
