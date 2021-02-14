// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactNotificationService.h>
#include <ReactPropertyBag.h>
#include <winrt/Windows.Foundation.h>
#include <functional>

#if defined(USE_WINUI3) && !defined(CORE_ABI) && !defined(__APPLE__) && \
    WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP | WINAPI_PARTITION_SYSTEM | WINAPI_PARTITION_GAMES)

#include <winrt/Microsoft.ReactNative.h>

namespace Microsoft::ReactNative {

inline auto GetPropertyNameForWindowMessage(UINT uMsg) {
  auto id = std::to_wstring(uMsg);
  winrt::Microsoft::ReactNative::ReactPropertyName windowMsgProperty{L"ReactNative.Desktop.WindowMessage", id.c_str()};
  return windowMsgProperty;
}

inline void ForwardWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {
  winrt::Microsoft::ReactNative::DesktopWindowMessage args{uMsg, wParam, lParam};
  svc.SendNotification(GetPropertyNameForWindowMessage(uMsg).Handle(), winrt::box_value((uint64_t)hwnd), args);
}

inline winrt::Microsoft::ReactNative::IReactNotificationSubscription SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    UINT uMsg,
    std::function<void(HWND, const winrt::Microsoft::ReactNative::DesktopWindowMessage &)> callback) {
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

inline std::unordered_map<UINT, winrt::Microsoft::ReactNative::IReactNotificationSubscription> SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    std::initializer_list<UINT> uMsgs,
    std::function<void(HWND, const winrt::Microsoft::ReactNative::DesktopWindowMessage &)> callback) {
  std::unordered_map<UINT, winrt::Microsoft::ReactNative::IReactNotificationSubscription> result;
  for (auto uMsg : uMsgs) {
    result[uMsg] = SubscribeToWindowMessage(svc, uMsg, callback);
  }
  return result;
}

#else
namespace Microsoft::ReactNative {

/// Stubs for UWP
template <typename... T>
void ForwardWindowMessage(const winrt::Microsoft::ReactNative::IReactNotificationService &svc, T &&...) {}

template <typename... T>
winrt::Microsoft::ReactNative::IReactNotificationSubscription SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    T &&...) {
  return nullptr;
}

template <typename... T>
std::unordered_map<UINT, winrt::Microsoft::ReactNative::IReactNotificationSubscription> SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    std::initializer_list<UINT> uMsgs,
    T &&...) {
  return std::unordered_map<UINT, winrt::Microsoft::ReactNative::IReactNotificationSubscription>{};
}
#endif

template <typename TFn, typename... TArgs>
auto CallIndirect(const wchar_t *dllName, const char *fnName, TArgs &&... args) {
  static void *library = nullptr;
  if (library == nullptr) {
    library = WINRT_IMPL_LoadLibraryW(dllName);
  }
  auto pfn = reinterpret_cast<TFn>(WINRT_IMPL_GetProcAddress(library, fnName));
  return pfn(args...);
}

#define CALL_INDIRECT(dllName, fn, ...) Microsoft::ReactNative::CallIndirect<decltype(&fn)>(dllName, #fn, __VA_ARGS__)

} // namespace Microsoft::ReactNative
