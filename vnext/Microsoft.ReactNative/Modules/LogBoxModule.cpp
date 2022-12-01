// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "LogBoxModule.h"

#include "IReactContext.h"
#include "ReactHost/ReactInstanceWin.h"
#include "ReactNativeHost.h"
#include "Utils/Helpers.h"
#include "XamlUtils.h"

#ifdef USE_FABRIC
#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <winrt/Windows.UI.Composition.h>
#endif
#include <UI.Xaml.Controls.Primitives.h>

namespace Microsoft::ReactNative {

#ifdef USE_FABRIC
constexpr PCWSTR c_logBoxWindowClassName = L"MS_REACTNATIVE_LOGBOX";
constexpr auto CompHostProperty = L"CompHost";
const int LOGBOX_DEFAULT_WIDTH = 700;
const int LOGBOX_DEFAULT_HEIGHT = 1000;
#endif // USE_FABRIC
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

#ifdef USE_FABRIC
LRESULT CALLBACK LogBoxWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  auto data = reinterpret_cast<::IUnknown *>(GetProp(hwnd, CompHostProperty));
  winrt::com_ptr<winrt::IUnknown> spunk;
  React::CompositionHwndHost host{nullptr};

  if (data) {
    winrt::check_hresult(data->QueryInterface(winrt::guid_of<React::CompositionHwndHost>(), winrt::put_abi(host)));
    auto result = static_cast<LRESULT>(host.TranslateMessage(message, wparam, lparam));
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
  wcex.cbWndExtra = sizeof(winrt::impl::abi<winrt::Microsoft::ReactNative::ICompositionHwndHost>::type *);
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszClassName = c_logBoxWindowClassName;
  ATOM classId = RegisterClassEx(&wcex);
  WINRT_VERIFY(classId);
  winrt::check_win32(!classId);

  registered = true;
}
#endif // USE_FABRIC

void LogBox::ShowOnUIThread() noexcept {
  auto host = React::implementation::ReactNativeHost::GetReactNativeHost(m_context.Properties());
  if (!host)
    return;

  if (!IsFabricEnabled(m_context.Properties().Handle())) {
    m_logBoxContent = React::ReactRootView();
    m_logBoxContent.ComponentName(L"LogBox");
    m_logBoxContent.ReactNativeHost(host);

    m_popup = xaml::Controls::Primitives::Popup{};
    xaml::FrameworkElement root{nullptr};

    if (Is19H1OrHigher()) {
      // XamlRoot added in 19H1 - is required to be set for XamlIsland scenarios
      if (auto xamlRoot = React::XamlUIService::GetXamlRoot(m_context.Properties().Handle())) {
        m_popup.XamlRoot(xamlRoot);
        root = xamlRoot.Content().as<xaml::FrameworkElement>();
      }
    }

    if (!root) {
      auto window = xaml::Window::Current();
      root = window.Content().as<xaml::FrameworkElement>();
    }

    m_logBoxContent.MaxHeight(root.ActualHeight());
    m_logBoxContent.Height(root.ActualHeight());
    m_logBoxContent.MaxWidth(root.ActualWidth());
    m_logBoxContent.Width(root.ActualWidth());
    m_logBoxContent.UpdateLayout();

    m_sizeChangedRevoker = root.SizeChanged(
        winrt::auto_revoke,
        [wkThis = weak_from_this()](auto const & /*sender*/, xaml::SizeChangedEventArgs const &args) {
          if (auto strongThis = wkThis.lock()) {
            strongThis->m_logBoxContent.MaxHeight(args.NewSize().Height);
            strongThis->m_logBoxContent.Height(args.NewSize().Height);
            strongThis->m_logBoxContent.MaxWidth(args.NewSize().Width);
            strongThis->m_logBoxContent.Width(args.NewSize().Width);
          }
        });

    m_tokenClosed = m_popup.Closed(
        [wkThis = weak_from_this()](auto const & /*sender*/, winrt::IInspectable const & /*args*/) noexcept {
          if (auto strongThis = wkThis.lock()) {
            strongThis->HideOnUIThread();
          }
        });

    m_popup.Child(m_logBoxContent);
    m_popup.IsOpen(true);
  }
#ifdef USE_FABRIC
  else {
    RegisterWndClass();

    if (!m_hwnd) {
      auto CompositionHwndHost = React::CompositionHwndHost();
      winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
      viewOptions.ComponentName(L"LogBox");
      CompositionHwndHost.ReactViewHost(
          winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));
      HINSTANCE hInstance = GetModuleHandle(NULL);
      winrt::impl::abi<winrt::Microsoft::ReactNative::ICompositionHwndHost>::type *pHost{nullptr};
      winrt::com_ptr<::IUnknown> spunk;
      CompositionHwndHost.as(spunk);
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
#endif // USE_FABRIC
}

void LogBox::HideOnUIThread() noexcept {
#ifdef USE_FABRIC
  if (m_hwnd) {
    ::ShowWindow(m_hwnd, SW_HIDE);
  }
#endif // USE_FABRIC
  if (m_popup) {
    m_popup.Closed(m_tokenClosed);
    m_sizeChangedRevoker.revoke();
    m_popup.IsOpen(false);
    m_popup = nullptr;
    m_logBoxContent = nullptr;
  }
}

void LogBox::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

} // namespace Microsoft::ReactNative
