
/*
 * This file is auto-generated from DrawingIslandNativeComponent spec file in flow / TypeScript.
 */
#pragma once

#include "NativeComponents.h"

namespace winrt::SampleCustomComponent::Codegen {

REACT_STRUCT(DrawingIslandProps)
struct DrawingIslandProps : winrt::implements<DrawingIslandProps, winrt::Microsoft::ReactNative::IComponentProps> {
  DrawingIslandProps(winrt::Microsoft::ReactNative::ViewProps props) : m_props(props) {}

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  winrt::Microsoft::ReactNative::ViewProps m_props;
};

struct DrawingIslandEventEmitter {
  DrawingIslandEventEmitter(const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter)
      : m_eventEmitter(eventEmitter) {}

 private:
  winrt::Microsoft::ReactNative::EventEmitter m_eventEmitter{nullptr};
};

template <typename TUserData, typename TProps = DrawingIslandProps, typename TEventEmitter = DrawingIslandEventEmitter>
void RegisterDrawingIslandNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      L"DrawingIsland", [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps(
            [](winrt::Microsoft::ReactNative::ViewProps props) noexcept { return winrt::make<TProps>(props); });

        RegisterUpdatePropsHandler<TUserData, TProps>(builder);
        RegisterUpdateEventEmitterHandler<TUserData, TEventEmitter>(builder);
        RegisterFinalizeUpdateHandler<TUserData>(builder);
        RegisterUpdateStateHandler<TUserData, TProps>(builder);
        RegisterCommandHandler<TUserData>(builder);
        RegisterMountChildComponentViewHandler<TUserData>(builder);
        RegisterUnmountChildComponentViewHandler<TUserData>(builder);

        RegisterComponentInitializer<TUserData>(compBuilder);
        RegisterComponentCreateVisual<TUserData>(compBuilder);

        // Allow app to further customize the builder
        if (builderCallback) {
          builderCallback(compBuilder);
        }
      });
}

} // namespace winrt::SampleCustomComponent::Codegen