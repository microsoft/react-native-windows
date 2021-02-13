// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <DesktopWindowBridge.h>
#include <ReactPropertyBag.h>

using namespace winrt::Microsoft::ReactNative;

#if !defined(CORE_ABI) && !defined(__APPLE__)
namespace Microsoft::ReactNative {

  auto GetPropertyNameForWindowMessage(UINT uMsg) {
    auto id = std::to_wstring(uMsg);
    ReactPropertyName windowMsgProperty{L"ReactNative.Desktop.WindowMessage", id.c_str()};
    return windowMsgProperty;
}

void ForwardWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
  winrt::Microsoft::ReactNative::DesktopWindowMessage args{uMsg, wParam, lParam};
  svc.SendNotification(GetPropertyNameForWindowMessage(uMsg).Handle(), winrt::box_value((uint64_t)hwnd), args);
}

std::unordered_map<UINT, winrt::Microsoft::ReactNative::IReactNotificationSubscription> SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    std::initializer_list<UINT> uMsgs,
    std::function<void(HWND, const winrt::Microsoft::ReactNative::DesktopWindowMessage &)> callback) {
  std::unordered_map<UINT, winrt::Microsoft::ReactNative::IReactNotificationSubscription> result;
  for (auto uMsg : uMsgs) {
    result[uMsg] = SubscribeToWindowMessage(svc, uMsg, callback);
  }
  return result;
}


  winrt::Microsoft::ReactNative::IReactNotificationSubscription SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    UINT uMsg,
    std::function<void(
        HWND,
        const winrt::Microsoft::ReactNative::DesktopWindowMessage &)> callback) {
  return svc.Subscribe(
      GetPropertyNameForWindowMessage(uMsg).Handle(),
      nullptr,
      [=](const winrt::Windows::Foundation::IInspectable &sender,
          const winrt::Microsoft::ReactNative::IReactNotificationArgs &args) {
        if (auto dwm = args.Data().try_as<winrt::Microsoft::ReactNative::DesktopWindowMessage>()) {
          callback(reinterpret_cast<HWND>(winrt::unbox_value<uint64_t>(sender)), dwm);
        }
      });
}

} // namespace winrt::Microsoft::ReactNative
#endif
