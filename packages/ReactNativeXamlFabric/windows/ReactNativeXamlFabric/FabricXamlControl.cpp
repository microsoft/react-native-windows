// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "FabricXamlControl.h"

#if defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)

#include <NativeModules.h>

#ifdef RNW_NEW_ARCH
#include <JSValueComposition.h>

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.UI.Composition.h>
#endif // #ifdef RNW_NEW_ARCH

#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::ReactNativeXamlFabric {

template <typename TUserData>
void CustomRegisterXamlControlNativeComponent(
    winrt::hstring name,
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      name, [name, builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps([name](winrt::Microsoft::ReactNative::ViewProps props,
                              const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom) noexcept {
            return winrt::make<FabricXamlControlProps>(name, props, cloneFrom); 
        });

        builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
            auto userData = view.UserData().as<TUserData>();
          userData->UpdateProps(
              view,
              newProps ? newProps.as<FabricXamlControlProps>() : nullptr,
              oldProps ? oldProps.as<FabricXamlControlProps>() : nullptr);
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
          userData->UpdateEventEmitter(std::make_shared<FabricXamlControlEventEmitter>(eventEmitter));
        });

        if constexpr (&TUserData::FinalizeUpdate != &winrt::ReactNativeXamlFabric::BaseFabricXamlControl<TUserData>::FinalizeUpdate) {
            builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->FinalizeUpdate(view, mask);
          });
        } 

        if constexpr (
            &TUserData::UpdateState !=
            &winrt::ReactNativeXamlFabric::BaseFabricXamlControl<TUserData>::UpdateState) {
          builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateState(view, newState);
          });
        }

        if constexpr (
            &TUserData::MountChildComponentView !=
            &winrt::ReactNativeXamlFabric::BaseFabricXamlControl<TUserData>::MountChildComponentView) {
          builder.SetMountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->MountChildComponentView(view, args);
          });
        }

        if constexpr (
            &TUserData::UnmountChildComponentView !=
            &winrt::ReactNativeXamlFabric::BaseFabricXamlControl<TUserData>::UnmountChildComponentView) {
          builder.SetUnmountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->UnmountChildComponentView(view, args);
          });
        }

        compBuilder.SetViewComponentViewInitializer([name](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
          auto userData = winrt::make_self<TUserData>(name);
              if constexpr (
                  &TUserData::Initialize !=
                  &winrt::ReactNativeXamlFabric::BaseFabricXamlControl<TUserData>::Initialize) {
            userData->Initialize(view);
          }
          view.UserData(*userData);
        });

        if constexpr (
            &TUserData::CreateVisual !=
            &winrt::ReactNativeXamlFabric::BaseFabricXamlControl<TUserData>::CreateVisual) {
          compBuilder.SetCreateVisualHandler([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
            // I suppose we should return null here since we're not backed by a visual?
            // No, it looks like in CompositionViewComponentView.cpp ViewComponentView::ensureVisual that this will fail.
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

}

void RegisterXamlControl(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder)
{  
  winrt::ReactNativeXamlFabric::CustomRegisterXamlControlNativeComponent<
      winrt::ReactNativeXamlFabric::XamlControlComponentView>(
      winrt::hstring{L"FX_StackPanel"},
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
              
        /*
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::ReactNativeXamlFabric::XamlControlComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
            */
        
      });

    winrt::ReactNativeXamlFabric::CustomRegisterXamlControlNativeComponent<
      winrt::ReactNativeXamlFabric::XamlControlComponentView>(
      winrt::hstring{L"FX_Button"},
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        /*
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::ReactNativeXamlFabric::XamlControlComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
            */
        
      });
}

#endif // defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
