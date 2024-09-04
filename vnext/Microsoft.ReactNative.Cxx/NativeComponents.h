// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>
#include <functional>
#include "JSValueComposition.h"
#include "JSValueReader.h"
#include "JSValueWriter.h"
#include "ModuleRegistration.h"
#include "ReactContext.h"

#define REACT_COMPONENT_UPDATE_PROPS(name) using _HasUpdatePropsMethod = std::true_type;
#define REACT_COMPONENT_UPDATE_STATE(name) using _HasUpdateStateMethod = std::true_type;
#define REACT_COMPONENT_FINALIZE_UPDATES(name) using _HasFinalizeUpdateMethod = std::true_type;
#define REACT_COMPONENT_UPDATE_EVENTEMITTER(name) using _HasUpdateEventEmitterMethod = std::true_type;
#define REACT_COMPONENT_HANDLE_COMMAND(name) using _HasCommandHandlerMethod = std::true_type;
#define REACT_COMPONENT_MOUNT_CHILD_COMPONENT_VIEW(name) using _HasMountChildComponentMethod = std::true ::type;
#define REACT_COMPONENT_UNMOUNT_CHILD_COMPONENT_VIEW(name) using _HasUnmountChildComponentMethod = std::true ::type;

#define REACT_COMPONENT_CONTENT_ISLAND_VIEW_INITIALIZE(name) \
  using _HasContentIslandViewInitializeMethod = std::true_type;
#define REACT_COMPONENT_VIEW_INITIALIZE(name) using _HasViewInitializeMethod = std::true_type;
#define REACT_COMPONENT_CREATE_VISUAL(name) using _HasCreateVisualMethod = std::true_type;

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
void RegisterUpdatePropsHandler(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
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
struct ReactHasUpdateStateHanderOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasUpdateStateHanderOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasUpdateStateMethod) >= 0)>> {
  using Type = typename TUserData::_HasUpdateStateMethod;
};
template <typename TUserData, typename TProps>
void RegisterUpdateStateHandler(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasUpdateStateHanderOrVoid<TUserData>::Type, void>) {
    builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept {
      auto userData = view.UserData().as<TUserData>();
      userData->UpdateState(view, newState);
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
void RegisterUpdateEventEmitterHandler(
    winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
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
    std::enable_if_t<(sizeof(typename TUserData::_HasViewInitializeMethod) >= 0)>> {
  using Type = typename TUserData::_HasViewInitializeMethod;
};
template <class TUserData, typename = void>
struct ReactHasContentIslandInitializeMethodOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasContentIslandInitializeMethodOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasContentIslandViewInitializeMethod) >= 0)>> {
  using Type = typename TUserData::_HasContentIslandViewInitializeMethod;
};
template <typename TUserData>
void RegisterComponentInitializer(
    winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder const &builder) noexcept {
  if constexpr (std::is_same_v<typename ReactHasContentIslandInitializeMethodOrVoid<TUserData>::Type, void>) {
    builder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
      auto userData = winrt::make_self<TUserData>();
      if constexpr (!std::is_same_v<typename ReactHasInitializeMethodOrVoid<TUserData>::Type, void>) {
        userData->Initialize(view);
      }
      view.UserData(*userData);
    });
  } else if (!std::is_same_v<typename ReactHasContentIslandInitializeMethodOrVoid<TUserData>::Type, void>) {
    builder.SetContentIslandComponentViewInitializer(
        [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &contentIslandView) noexcept {
          auto userData = winrt::make_self<TUserData>();
          userData->Initialize(contentIslandView);
          contentIslandView.UserData(*userData);
        });
  }
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
void RegisterComponentCreateVisual(
    winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasCreateVisualMethodOrVoid<TUserData>::Type, void>) {
    builder.SetCreateVisualHandler([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
      auto userData = view.UserData().as<TUserData>();
      return userData->CreateVisual(view);
    });
  }
}

template <class TUserData, typename = void>
struct ReactHasCommandHandlerMethodOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasCommandHandlerMethodOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasCommandHandlerMethod) >= 0)>> {
  using Type = typename TUserData::_HasCommandHandlerMethod;
};
template <typename TUserData>
void RegisterCommandHandler(winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasCommandHandlerMethodOrVoid<TUserData>::Type, void>) {
    builder.SetCustomCommandHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                       winrt::hstring commandName,
                                       const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept {
      auto userData = view.UserData().as<TUserData>();
      return userData->HandleCommand(view, commandName, args);
    });
  }
}

template <class TUserData, typename = void>
struct ReactHasMountChildComponentHandlerMethodOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasMountChildComponentHandlerMethodOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasMountChildComponentMethod) >= 0)>> {
  using Type = typename TUserData::_HasMountChildComponentMethod;
};
template <typename TUserData>
void RegisterMountChildComponentViewHandler(
    winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasMountChildComponentHandlerMethodOrVoid<TUserData>::Type, void>) {
    builder.SetMountChildComponentViewHandler(
        [](const winrt::Microsoft::ReactNative::ComponentView &view,
           const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
          auto userData = view.UserData().as<TUserData>();
          return userData->MountChildComponentView(view, args);
        });
  }
}

template <class TUserData, typename = void>
struct ReactHasUnmountChildComponentHandlerMethodOrVoid {
  using Type = void;
};
template <class TUserData>
struct ReactHasUnmountChildComponentHandlerMethodOrVoid<
    TUserData,
    std::enable_if_t<(sizeof(typename TUserData::_HasUnmountChildComponentMethod) >= 0)>> {
  using Type = typename TUserData::_HasUnmountChildComponentMethod;
};
template <typename TUserData>
void RegisterUnmountChildComponentViewHandler(
    winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
  if constexpr (!std::is_same_v<typename ReactHasUnmountChildComponentHandlerMethodOrVoid<TUserData>::Type, void>) {
    builder.SetUnmountChildComponentViewHandler(
        [](const winrt::Microsoft::ReactNative::ComponentView &view,
           const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
          auto userData = view.UserData().as<TUserData>();
          return userData->UnmountChildComponentView(view, args);
        });
  }
}
