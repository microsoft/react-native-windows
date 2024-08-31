// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
// IMPORTANT: Before updating this file
// please read react-native-windows repo:
// vnext/Microsoft.ReactNative.Cxx/README.md

#pragma once
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.Foundation.h>
#include "JSValueReader.h"
#include "JSValueWriter.h"
#include "JSValueComposition.h"
#include "ModuleRegistration.h"
#include "ReactContext.h"
#include "ReactNonAbiValue.h"
#include "ReactPromise.h"

#include <functional>
#include <type_traits>

#define REACT_COMPONENT_UPDATE_PROPS(name) using _HasUpdatePropsMethod = std::true_type;
#define REACT_COMPONENT_FINALIZE_UPDATES(name) using _HasFinalizeUpdateMethod = std::true_type;
#define REACT_COMPONENT_INITIALIZE(name) using _HasInitializeMethod = std::true_type;
#define REACT_COMPONENT_CREATE_VISUAL(name) using _HasCreateVisualMethod = std::true_type;
#define REACT_COMPONENT_UPDATE_EVENTEMITTER(name) using _HasUpdateEventEmitterMethod = std::true_type;


template <class TUserData, typename = void>
struct ReactHasUpdatePropsHanderOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasUpdatePropsHanderOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasUpdatePropsMethod) >= 0)>> {
  using Type = typename TUserData::_HasUpdatePropsMethod;
};
template <typename TUserData, typename TProps>
void RegisterUpdatePropsHandler(
        winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {

  if constexpr (!std::is_same_v<typename ReactHasUpdatePropsHanderOrVoid<TUserData>::Type, void>) {
    builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
      auto userData = view.UserData().as<TUserData>();
      userData->UpdateProps(
          view, newProps ? newProps.as<TProps>() : nullptr, oldProps ? oldProps.as<TProps>() : nullptr);
    });
  }
}


template <class TUserData, typename = void>
struct ReactHasUpdateEventEmiiterHanderOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasUpdateEventEmiiterHanderOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasUpdateEventEmitterMethod) >= 0)>> {
  using Type = typename TUserData::_HasUpdateEventEmitterMethod;
};
template <typename TUserData, typename TEventEmitter>
void RegisterUpdateEventEmitterHandler(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasUpdateEventEmiiterHanderOrVoid<TUserData>::Type, void>) {
    builder.SetUpdateEventEmitterHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter) noexcept {
      auto userData = view.UserData().as<TUserData>();
      userData->UpdateEventEmitter(std::make_shared<TEventEmitter>(eventEmitter));
    });
  }
}


template <class TUserData, typename = void>
struct ReactHasFinalizeUpdateMethodOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasFinalizeUpdateMethodOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasFinalizeUpdateMethod) >= 0)>> {
  using Type = typename TUserData::_HasFinalizeUpdateMethod;
};
template <typename TUserData>
void RegisterFinalizeUpdateHandler(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasFinalizeUpdateMethodOrVoid<TUserData>::Type, void>) {
    builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                        winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
      auto userData = view.UserData().as<TUserData>();
      userData->FinalizeUpdates(view, mask);
    });
  }
}

template <class TUserData, typename = void>
struct ReactHasInitializeMethodOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasInitializeMethodOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasInitializeMethod) >= 0)>> {
  using Type = typename TUserData::_HasInitializeMethod;
};
template <typename TUserData>
void RegisterComponentInitializer(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();
  compBuilder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    auto userData = winrt::make_self<TUserData>();
    if constexpr (!std::is_same_v<typename ReactHasInitializeMethodOrVoid<TUserData>::Type, void>) {
      userData->Initialize(view);
    }
    view.UserData(*userData);
  });
}

template <class TUserData, typename = void>
struct ReactHasCreateVisualMethodOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasCreateVisualMethodOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasCreateVisualMethod) >= 0)>> {
  using Type = typename TUserData::_HasCreateVisualMethod;
};
template <typename TUserData>
void RegisterComponentCreateVisual(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasCreateVisualMethodOrVoid<TUserData>::Type, void>) {
    auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();
    compBuilder.SetCreateVisualHandler([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
      auto userData = view.UserData().as<TUserData>();
      return userData->CreateVisual(view);
    });
  }
}

