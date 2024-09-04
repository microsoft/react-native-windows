// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>
#include <functional>
#include <type_traits>
#include "JSValueComposition.h"
#include "JSValueReader.h"
#include "JSValueWriter.h"
#include "ModuleRegistration.h"
#include "ReactContext.h"

#define REACT_COMPONENT_UPDATE_PROPS(member)                                                                          \
  template <typename TUserData, typename TProps>                                                                      \
  static void RegisterUpdatePropsHandler(                                                                             \
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {                            \
    builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,                        \
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,                  \
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {       \
      auto userData = view.UserData().as<TUserData>();                                                                \
      userData->member(view, newProps ? newProps.as<TProps>() : nullptr, oldProps ? oldProps.as<TProps>() : nullptr); \
    });                                                                                                               \
  }

#define REACT_COMPONENT_UPDATE_STATE(member)                                                                    \
  template <typename TUserData, typename TProps>                                                                \
  static void RegisterUpdateStateHandler(                                                                       \
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {                      \
    builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,                  \
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept { \
      auto userData = view.UserData().as<TUserData>();                                                          \
      userData->member(view, newState);                                                                         \
    });                                                                                                         \
  }

#define REACT_COMPONENT_UPDATE_EVENTEMITTER(member)                                        \
  template <typename TUserData, typename TEventEmitter>                                    \
  static void RegisterUpdateEventEmitterHandler(                                           \
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept { \
    builder.SetUpdateEventEmitterHandler(                                                  \
        [](const winrt::Microsoft::ReactNative::ComponentView &view,                       \
           const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter) noexcept {     \
          auto userData = view.UserData().as<TUserData>();                                 \
          userData->member(std::make_shared<TEventEmitter>(eventEmitter));                 \
        });                                                                                \
  }

#define REACT_COMPONENT_HANDLE_COMMAND(member)                                                               \
  template <typename TUserData>                                                                              \
  static void RegisterCommandHandler(                                                                        \
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {                   \
    builder.SetCustomCommandHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,             \
                                       winrt::hstring commandName,                                           \
                                       const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept { \
      auto userData = view.UserData().as<TUserData>();                                                       \
      return userData->member(view, commandName, args);                                                      \
    });                                                                                                      \
  }

#define REACT_COMPONENT_MOUNT_CHILD_COMPONENT_VIEW(member)                                    \
  template <typename TUserData>                                                               \
  static void RegisterMountChildComponentViewHandler(                                         \
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {    \
    builder.SetMountChildComponentViewHandler(                                                \
        [](const winrt::Microsoft::ReactNative::ComponentView &view,                          \
           const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept { \
          auto userData = view.UserData().as<TUserData>();                                    \
          return userData->member(view, args);                                                \
        });                                                                                   \
  }

#define REACT_COMPONENT_UNMOUNT_CHILD_COMPONENT_VIEW(member)                                    \
  template <typename TUserData>                                                                 \
  static void RegisterUnmountChildComponentViewHandler(                                         \
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {      \
    builder.SetUnmountChildComponentViewHandler(                                                \
        [](const winrt::Microsoft::ReactNative::ComponentView &view,                            \
           const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept { \
          auto userData = view.UserData().as<TUserData>();                                      \
          return userData->member(view, args);                                                  \
        });                                                                                     \
  }

#define REACT_COMPONENT_VIEW_INITIALIZE(member)                                                                     \
  template <typename TUserData>                                                                                     \
  static void RegisterComponentInitializer(                                                                         \
      winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder const &builder) noexcept {  \
    builder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept { \
      auto userData = winrt::make_self<TUserData>();                                                                \
      userData->member(view);                                                                                       \
      view.UserData(*userData);                                                                                     \
    });                                                                                                             \
  }

#define REACT_COMPONENT_CONTENT_ISLAND_VIEW_INITIALIZE(member)                                                         \
  template <typename TUserData>                                                                                        \
  static void RegisterComponentInitializer(                                                                            \
      winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder const &builder) noexcept {     \
    builder.SetContentIslandComponentViewInitializer(                                                                  \
        [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &contentIslandView) noexcept { \
          auto userData = winrt::make_self<TUserData>();                                                               \
          userData->member(contentIslandView);                                                                         \
          contentIslandView.UserData(*userData);                                                                       \
        });                                                                                                            \
  }

#define REACT_COMPONENT_CREATE_VISUAL(member)                                                                      \
  template <typename TUserData>                                                                                    \
  static void RegisterComponentCreateVisual(                                                                       \
      winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder const &builder) noexcept { \
    builder.SetCreateVisualHandler([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {         \
      auto userData = view.UserData().as<TUserData>();                                                             \
      return userData->member(view);                                                                               \
    });                                                                                                            \
  }

#define REACT_COMPONENT_FINALIZE_UPDATES(member)                                                          \
  template <class TUserData>                                                                              \
  static void RegisterFinalizeUpdateHandler(                                                              \
      winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {                \
    builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,         \
                                  winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept { \
      auto userData = view.UserData().as<TUserData>();                                                    \
      userData->member(view, mask);                                                                       \
    });                                                                                                   \
  }
