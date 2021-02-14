// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <ReactNotificationService.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>
#include <functional>

namespace Microsoft::ReactNative {
void ForwardWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);

winrt::Microsoft::ReactNative::IReactNotificationSubscription SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    UINT uMsg,
    std::function<void(HWND sender, const winrt::Microsoft::ReactNative::DesktopWindowMessage &)> callback);

std::unordered_map<UINT, winrt::Microsoft::ReactNative::IReactNotificationSubscription> SubscribeToWindowMessage(
    const winrt::Microsoft::ReactNative::IReactNotificationService &svc,
    std::initializer_list<UINT> uMsgs,
    std::function<void(HWND, const winrt::Microsoft::ReactNative::DesktopWindowMessage &)> callback);

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

namespace winrt::impl {

template <>
struct reference_traits<MSG> {
  static auto make(const MSG &msg) {
    return winrt::Microsoft::ReactNative::DesktopWindowMessage{msg.message, msg.wParam, msg.lParam};
  }
};
} // namespace winrt::impl
