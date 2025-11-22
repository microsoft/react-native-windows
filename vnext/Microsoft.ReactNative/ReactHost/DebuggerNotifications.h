// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.h>

namespace Microsoft::ReactNative {

struct DebuggerNotifications {
  static winrt::Microsoft::ReactNative::IReactPropertyName ShowDebuggerPausedOverlayEventName() noexcept {
    static winrt::Microsoft::ReactNative::IReactPropertyName propertyName{
        winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
            winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.Debugger"),
            L"ShowDebuggerPausedOverlay")};
    return propertyName;
  }

  static void OnShowDebuggerPausedOverlay(
      winrt::Microsoft::ReactNative::IReactNotificationService const &service,
      std::string message,
      std::function<void()> onResume) {
    const winrt::Microsoft::ReactNative::ReactNonAbiValue<std::tuple<std::string, std::function<void()>>> nonAbiValue{
        std::in_place, std::tie(message, onResume)};
    service.SendNotification(ShowDebuggerPausedOverlayEventName(), nullptr, nonAbiValue);
  }

  static void OnHideDebuggerPausedOverlay(winrt::Microsoft::ReactNative::IReactNotificationService const &service) {
    service.SendNotification(ShowDebuggerPausedOverlayEventName(), nullptr, nullptr);
  }

  static winrt::Microsoft::ReactNative::IReactNotificationSubscription SubscribeShowDebuggerPausedOverlay(
      winrt::Microsoft::ReactNative::IReactNotificationService const &service,
      winrt::Microsoft::ReactNative::IReactDispatcher const &dispatcher,
      std::function<void(std::string, std::function<void()>)> showCallback,
      std::function<void()> hideCallback) {
    return service.Subscribe(
        ShowDebuggerPausedOverlayEventName(),
        dispatcher,
        [showCallback, hideCallback](auto &&, winrt::Microsoft::ReactNative::IReactNotificationArgs const &args) {
          if (args.Data()) {
            const auto [message, onResume] = args.Data()
                                                 .as<winrt::Microsoft::ReactNative::ReactNonAbiValue<
                                                     std::tuple<std::string, std::function<void()>>>>()
                                                 .Value();
            showCallback(message, onResume);
          } else {
            hideCallback();
          }
        });
  }
};

} // namespace Microsoft::ReactNative
