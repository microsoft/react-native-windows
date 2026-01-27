
/*
 * This file is auto-generated from AndroidHorizontalScrollContentViewNativeComponent spec file in flow / TypeScript.
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

REACT_STRUCT(AndroidHorizontalScrollContentViewProps)
struct AndroidHorizontalScrollContentViewProps : winrt::implements<AndroidHorizontalScrollContentViewProps, winrt::Microsoft::ReactNative::IComponentProps> {
  AndroidHorizontalScrollContentViewProps(winrt::Microsoft::ReactNative::ViewProps props, const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom)
    : ViewProps(props)
  {
     if (cloneFrom) {
       auto cloneFromProps = cloneFrom.as<AndroidHorizontalScrollContentViewProps>();
       removeClippedSubviews = cloneFromProps->removeClippedSubviews;  
     }
  }

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(removeClippedSubviews)
  std::optional<bool> removeClippedSubviews{};

  const winrt::Microsoft::ReactNative::ViewProps ViewProps;
};

struct AndroidHorizontalScrollContentViewEventEmitter {
  AndroidHorizontalScrollContentViewEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

template<typename TUserData>
struct BaseAndroidHorizontalScrollContentView {

  virtual void UpdateProps(
    const winrt::Microsoft::ReactNative::ComponentView &/*view*/,
    const winrt::com_ptr<AndroidHorizontalScrollContentViewProps> &newProps,
    const winrt::com_ptr<AndroidHorizontalScrollContentViewProps> &/*oldProps*/) noexcept {
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

  virtual void UpdateEventEmitter(const std::shared_ptr<AndroidHorizontalScrollContentViewEventEmitter> &eventEmitter) noexcept {
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

  

  const std::shared_ptr<AndroidHorizontalScrollContentViewEventEmitter>& EventEmitter() const { return m_eventEmitter; }
  const winrt::com_ptr<AndroidHorizontalScrollContentViewProps>& Props() const { return m_props; }

private:
  winrt::com_ptr<AndroidHorizontalScrollContentViewProps> m_props;
  std::shared_ptr<AndroidHorizontalScrollContentViewEventEmitter> m_eventEmitter;
};

template <typename TUserData>
void RegisterAndroidHorizontalScrollContentViewNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"AndroidHorizontalScrollContentView", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props,
                              const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom) noexcept {
            return winrt::make<AndroidHorizontalScrollContentViewProps>(props, cloneFrom); 
        });

        builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateProps(view, newProps ? newProps.as<AndroidHorizontalScrollContentViewProps>() : nullptr, oldProps ? oldProps.as<AndroidHorizontalScrollContentViewProps>() : nullptr);
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
          userData->UpdateEventEmitter(std::make_shared<AndroidHorizontalScrollContentViewEventEmitter>(eventEmitter));
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::FinalizeUpdate != &BaseAndroidHorizontalScrollContentView<TUserData>::FinalizeUpdate) {
            builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->FinalizeUpdate(view, mask);
          });
        } 

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UpdateState != &BaseAndroidHorizontalScrollContentView<TUserData>::UpdateState) {
          builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateState(view, newState);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::MountChildComponentView != &BaseAndroidHorizontalScrollContentView<TUserData>::MountChildComponentView) {
          builder.SetMountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->MountChildComponentView(view, args);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::UnmountChildComponentView != &BaseAndroidHorizontalScrollContentView<TUserData>::UnmountChildComponentView) {
          builder.SetUnmountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->UnmountChildComponentView(view, args);
          });
        }

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::CreateAutomationPeer != &BaseAndroidHorizontalScrollContentView<TUserData>::CreateAutomationPeer) {
            builder.SetCreateAutomationPeerHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::CreateAutomationPeerArgs& args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->CreateAutomationPeer(view, args);
          });
        } 

        compBuilder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
          auto userData = winrt::make_self<TUserData>();
          if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::Initialize != &BaseAndroidHorizontalScrollContentView<TUserData>::Initialize) {
            userData->Initialize(view);
          }
          view.UserData(*userData);
        });

        if CONSTEXPR_SUPPORTED_ON_VIRTUAL_FN_ADDRESS (&TUserData::CreateVisual != &BaseAndroidHorizontalScrollContentView<TUserData>::CreateVisual) {
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
