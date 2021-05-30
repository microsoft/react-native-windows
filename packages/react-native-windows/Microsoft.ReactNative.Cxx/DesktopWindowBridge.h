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
  ReactNotificationId<DesktopWindowMessage> windowMsgNotificationId{L"ReactNative.Desktop.WindowMessage", id.c_str()};
  return windowMsgNotificationId;
}
} // namespace details

inline void ForwardWindowMessage(
    const ReactNotificationService &svc,
    HWND hwnd,
    uint32_t msg,
    WPARAM wParam,
    LPARAM lParam) noexcept {
  DesktopWindowMessage args{msg, wParam, lParam};
  svc.SendNotification(details::GetNotificationIdForWindowMessage(msg), winrt::box_value((uint64_t)hwnd), args);
}

inline ReactNotificationSubscription SubscribeToWindowMessage(
    const ReactNotificationService &svc,
    uint32_t msg,
    std::function<void(HWND, const DesktopWindowMessage &)> &&callback) noexcept {
  return svc.Subscribe(
      details::GetNotificationIdForWindowMessage(msg),
      [cb = std::move(callback)](
          const winrt::Windows::Foundation::IInspectable &sender,
          const ReactNotificationArgs<DesktopWindowMessage> &args) {
        if (auto dwm = args.Data()) {
          if (dwm.has_value()) {
            cb(reinterpret_cast<HWND>(winrt::unbox_value<uint64_t>(sender)), dwm.value());
          }
        }
      });
}

inline std::unordered_map<uint32_t, ReactNotificationSubscription> SubscribeToWindowMessage(
    const ReactNotificationService &svc,
    std::initializer_list<uint32_t> msgs,
    std::function<void(HWND, const DesktopWindowMessage &)> &&callback) noexcept {
  std::unordered_map<uint32_t, ReactNotificationSubscription> result;
  for (auto msg : msgs) {
    result[msg] = SubscribeToWindowMessage(svc, msg, std::move(callback));
  }
  return result;
}

#else
namespace winrt::Microsoft::ReactNative {

/// Stubs for UWP
template <typename... T>
void ForwardWindowMessage(const ReactNotificationService &svc, T &&...) noexcept {}

template <typename... T>
ReactNotificationSubscription SubscribeToWindowMessage(const ReactNotificationService &svc, T &&...) noexcept {
  return nullptr;
}

template <typename... T>
std::unordered_map<uint32_t, ReactNotificationSubscription>
SubscribeToWindowMessage(const ReactNotificationService &svc, std::initializer_list<uint32_t> msgs, T &&...) noexcept {
  return std::unordered_map<uint32_t, ReactNotificationSubscription>{};
}
#endif

namespace details {
struct IndirectLibraryDeleter {
  void operator()(void *l) {
    WINRT_IMPL_FreeLibrary(l);
  }
};

using IndirectLibrary = std::unique_ptr<void, IndirectLibraryDeleter>;
__declspec(selectany) std::unordered_map<std::wstring, IndirectLibrary> indirectLibraries{};
} // namespace details

template <typename TFn, typename... TArgs>
auto CallIndirect(const wchar_t *dllName, const char *fnName, TArgs &&... args) noexcept {
  if (details::indirectLibraries.count(dllName) == 0) {
    details::indirectLibraries.emplace(dllName, WINRT_IMPL_LoadLibraryW(dllName));
  }
  auto &library = details::indirectLibraries[dllName];
  auto pfn = reinterpret_cast<TFn>(WINRT_IMPL_GetProcAddress(library.get(), fnName));
  return pfn(args...);
}

#define CALL_INDIRECT(dllName, fn, ...) \
  winrt::Microsoft::ReactNative::CallIndirect<decltype(&fn)>(dllName, #fn, __VA_ARGS__)

} // namespace winrt::Microsoft::ReactNative
