// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "LogBoxModule.h"

#include "IReactContext.h"
#include "ReactHost/ReactInstanceWin.h"
#include "ReactNativeHost.h"
#include "Utils/Helpers.h"

#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

constexpr PCWSTR c_logBoxWindowClassName = L"MS_REACTNATIVE_LOGBOX";
constexpr auto CompHostProperty = L"CompHost";
const int LOGBOX_DEFAULT_WIDTH = 700;
const int LOGBOX_DEFAULT_HEIGHT = 1000;

void LogBox::Show() noexcept {
  if (!Mso::React::ReactOptions::UseDeveloperSupport(m_context.Properties().Handle())) {
    return;
  }

  m_context.UIDispatcher().Post([weakThis = weak_from_this()] {
    if (auto strongThis = weakThis.lock()) {
      strongThis->ShowOnUIThread();
    }
  });
}

void LogBox::Hide() noexcept {
  m_context.UIDispatcher().Post([weakThis = weak_from_this()] {
    if (auto strongThis = weakThis.lock()) {
      strongThis->HideOnUIThread();
    }
  });
}

bool g_ShouldBeWindowProp_isShown = false;

LRESULT CALLBACK LogBoxWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  auto data = reinterpret_cast<::IUnknown *>(GetProp(hwnd, CompHostProperty));
  winrt::com_ptr<winrt::IUnknown> spunk;
  React::CompHwndHost host{nullptr};

  if (data) {
    winrt::check_hresult(data->QueryInterface(winrt::guid_of<React::CompHwndHost>(), winrt::put_abi(host)));
    auto result = host.TranslateMessage(message, wparam, lparam);
    if (result)
      return result;
  }

  switch (message) {
    case WM_NCCREATE: {
      auto cs = reinterpret_cast<CREATESTRUCT *>(lparam);
      auto windowData = static_cast<::IUnknown *>(cs->lpCreateParams);
      SetProp(hwnd, CompHostProperty, reinterpret_cast<::IUnknown *>(windowData));
      break;
    }
    case WM_CREATE: {
      host.Compositor(winrt::Windows::UI::Composition::Compositor());
      host.Initialize((uint64_t)hwnd);
      break;
    }
    case WM_CLOSE: {
      // Just hide the window instead of destroying it
      ::ShowWindow(hwnd, SW_HIDE);
      return 0;
    }
    case WM_DESTROY: {
      data->Release();
      SetProp(hwnd, CompHostProperty, nullptr);
      g_ShouldBeWindowProp_isShown = false;
    }
  }

  return DefWindowProc(hwnd, message, wparam, lparam);
}

void LogBox::RegisterWndClass() noexcept {
  static bool registered = false;
  if (registered)
    return;

  HINSTANCE hInstance = GetModuleHandle(NULL);

  WNDCLASSEXW wcex = {};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = &LogBoxWndProc;
  wcex.cbClsExtra = DLGWINDOWEXTRA;
  wcex.cbWndExtra = sizeof(winrt::impl::abi<winrt::Microsoft::ReactNative::ICompHwndHost>::type *);
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  // wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLAYGROUND_WIN32);
  wcex.lpszClassName = c_logBoxWindowClassName;
  // wcex.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
  ATOM classId = RegisterClassEx(&wcex);
  WINRT_VERIFY(classId);
  winrt::check_win32(!classId);

  registered = true;
}

void LogBox::ShowOnUIThread() noexcept {
  auto host = React::implementation::ReactNativeHost::GetReactNativeHost(m_context.Properties());
  if (!host)
    return;

  RegisterWndClass();

  if (!m_hwnd) {
    auto compHwndHost = React::CompHwndHost();
    compHwndHost.ComponentName(L"LogBox");
    compHwndHost.ReactNativeHost(host);
    HINSTANCE hInstance = GetModuleHandle(NULL);
    winrt::impl::abi<winrt::Microsoft::ReactNative::ICompHwndHost>::type *pHost{nullptr};
    winrt::com_ptr<::IUnknown> spunk;
    compHwndHost.as(spunk);
    spunk->AddRef(); // Will be stored in windowData

    m_hwnd = CreateWindow(
        c_logBoxWindowClassName,
        L"React-Native LogBox",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        LOGBOX_DEFAULT_WIDTH,
        LOGBOX_DEFAULT_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        spunk.get());
  }

  ShowWindow(m_hwnd, SW_NORMAL);
  BringWindowToTop(m_hwnd);
  SetFocus(m_hwnd);
}

void LogBox::HideOnUIThread() noexcept {
  if (m_hwnd) {
    ::ShowWindow(m_hwnd, SW_HIDE);
  }
}

void LogBox::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

} // namespace Microsoft::ReactNative
