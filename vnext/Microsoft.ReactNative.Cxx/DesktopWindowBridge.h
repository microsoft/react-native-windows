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

namespace winrt::Microsoft::ReactNative {

  namespace details {
  inline auto GetNotificationIdForWindowMessage(uint32_t msg) noexcept {
    auto id = std::to_wstring(msg);
    winrt::Microsoft::ReactNative::ReactNotificationId<winrt::Microsoft::ReactNative::DesktopWindowMessage>
        windowMsgNotificationId{L"ReactNative.Desktop.WindowMessage", id.c_str()};
    return windowMsgNotificationId;
  }
} // namespace details

inline void ForwardWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    HWND hwnd,
    uint32_t msg,
    WPARAM wParam,
    LPARAM lParam) noexcept {
  winrt::Microsoft::ReactNative::DesktopWindowMessage args{msg, wParam, lParam};
  svc.SendNotification(details::GetNotificationIdForWindowMessage(msg).Handle(), winrt::box_value((uint64_t)hwnd), args);
}

inline winrt::Microsoft::ReactNative::IReactNotificationSubscription SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    uint32_t msg,
    std::function<void(HWND, const winrt::Microsoft::ReactNative::DesktopWindowMessage &)>&& callback) noexcept {
  return svc.Subscribe(
      details::GetNotificationIdForWindowMessage(msg).Handle(),
      nullptr,
      [cb = std::move(callback)](const winrt::Windows::Foundation::IInspectable &sender,
          const winrt::Microsoft::ReactNative::IReactNotificationArgs& args) {
        if (auto dwm = args.Data().try_as<winrt::Microsoft::ReactNative::DesktopWindowMessage>()) {
          cb(reinterpret_cast<HWND>(winrt::unbox_value<uint64_t>(sender)), dwm);
        }
      });
}

inline std::unordered_map<uint32_t, winrt::Microsoft::ReactNative::IReactNotificationSubscription> SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    std::initializer_list<uint32_t> msgs,
    std::function<void(HWND, const winrt::Microsoft::ReactNative::DesktopWindowMessage &)>&& callback) noexcept {
  std::unordered_map<uint32_t, winrt::Microsoft::ReactNative::IReactNotificationSubscription> result;
  for (auto msg : msgs) {
    result[msg] = SubscribeToWindowMessage(svc, msg, std::move(callback));
  }
  return result;
}

#else
namespace Microsoft::ReactNative {

/// Stubs for UWP
template <typename... T>
void ForwardWindowMessage(const winrt::Microsoft::ReactNative::IReactNotificationService &svc, T &&...) noexcept {}

template <typename... T>
winrt::Microsoft::ReactNative::ReactNotificationSubscription SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    T &&...) noexcept {
  return nullptr;
}

template <typename... T>
std::unordered_map<uint32_t, winrt::Microsoft::ReactNative::ReactNotificationSubscription> SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    std::initializer_list<uint32_t> msgs,
    T &&...) noexcept {
  return std::unordered_map<uint32_t, winrt::Microsoft::ReactNative::IReactNotificationSubscription>{};
}
#endif

template <typename TFn, typename... TArgs>
auto CallIndirect(const wchar_t *dllName, const char *fnName, TArgs &&...args) noexcept {
  static void *library = nullptr;
  if (library == nullptr) {
    library = WINRT_IMPL_LoadLibraryW(dllName);
  }
  auto pfn = reinterpret_cast<TFn>(WINRT_IMPL_GetProcAddress(library, fnName));
  return pfn(args...);
}

#define CALL_INDIRECT(dllName, fn, ...) winrt::Microsoft::ReactNative::CallIndirect<decltype(&fn)>(dllName, #fn, __VA_ARGS__)

} // namespace Microsoft::ReactNative
