#include "pch.h"
#include "resource.h"

#include <shobjidl.h>
#include <windows.h>

#include <ReactUWP/InstanceFactory.h>
#include <ReactUWP/ReactUwp.h>
#include <Unicode.h>

#include <react-native-windows-extended.h>

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <Windows.UI.Xaml.Hosting.DesktopWindowXamlSource.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>

#pragma pop_macro("GetCurrentTime")

namespace WUXH = winrt::Windows::UI::Xaml::Hosting;

struct PlaygroundNativeModuleProvider final : facebook::react::NativeModuleProvider {
  virtual std::vector<facebook::react::NativeModuleDescription> GetModules(
      const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread) override {
    std::vector<facebook::react::NativeModuleDescription> modules;
    std::shared_ptr<facebook::react::MessageQueueThread> queue = defaultQueueThread;

    // modules.emplace_back(SampleCxxModule::Name, []()
    //    {
    //        return std::make_unique<SampleCxxModule>();
    //    }, queue);

    return modules;
  }
};

class PlaygroundViewManagerProvider final : public react::uwp::ViewManagerProvider {
 public:
  virtual std::vector<react::uwp::NativeViewManager> GetViewManagers(
      const std::shared_ptr<react::uwp::IReactInstance> &instance) override {
    std::vector<react::uwp::NativeViewManager> viewManagers;

    viewManagers.emplace_back(react_native_windows_extended::CreateCustomViewManager(instance));

    return viewManagers;
  }
};

struct WindowData {
  static HINSTANCE s_instance;

  std::wstring m_bundleFile;
  WUXH::DesktopWindowXamlSource m_desktopWindowXamlSource;
  std::shared_ptr<react::uwp::IReactInstance> m_instance;

  bool m_useWebDebugger{true};
  bool m_liveReloadEnabled{true};
  bool m_reuseInstance{true};

  WindowData(const WUXH::DesktopWindowXamlSource &desktopWindowXamlSource)
      : m_desktopWindowXamlSource(desktopWindowXamlSource) {}

  static WindowData *GetFromWindow(HWND hwnd) {
    return reinterpret_cast<WindowData *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  }

  LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT) {
    switch (id) {
      case IDM_OPENJSFILE: {
        DialogBox(s_instance, MAKEINTRESOURCE(IDD_OPENJSBUNDLEBOX), hwnd, &Bundle);

        if (!m_bundleFile.empty()) {
          facebook::react::InitializeLogging(
              [](facebook::react::RCTLogLevel logLevel, const char *message) { OutputDebugStringA(message); });

          // Create NativeModuleProvider
          std::shared_ptr<facebook::react::NativeModuleProvider> moduleLoader =
              std::make_shared<PlaygroundNativeModuleProvider>();
          std::shared_ptr<react::uwp::ViewManagerProvider> viewManagerProvider =
              std::make_shared<PlaygroundViewManagerProvider>();

          m_instance = react::uwp::CreateReactInstance(moduleLoader, viewManagerProvider);

          react::uwp::ReactInstanceSettings settings;
          settings.UseWebDebugger = m_useWebDebugger;
          settings.UseLiveReload = m_liveReloadEnabled;
          settings.EnableDeveloperMenu = true;

          settings.LoggingCallback = [](facebook::react::RCTLogLevel logLevel, const char *message) {
            OutputDebugStringA("In LoggingCallback");
            OutputDebugStringA(message);
          };

          settings.JsExceptionCallback = [](facebook::react::JSExceptionInfo &&exceptionInfo) {
            OutputDebugStringA("in JsExceptionCallback");
          };

          m_instance->Start(m_instance, settings);
          m_instance->loadBundle(Microsoft::Common::Unicode::Utf16ToUtf8(m_bundleFile));
        }

        break;
      }
      case IDM_ABOUT:
        DialogBox(s_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, &About);
        break;
      case IDM_EXIT:
        PostQuitMessage(0);
        break;
      case IDM_REFRESH:
        break;
      case IDM_SETTINGS:
        DialogBoxParam(s_instance, MAKEINTRESOURCE(IDD_SETTINGSBOX), hwnd, &Settings, reinterpret_cast<INT_PTR>(this));
    }

    return 0;
  }

  LRESULT OnCreate(HWND hwnd, LPCREATESTRUCT createStruct) {
    auto interop = m_desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
    // Parent the DesktopWindowXamlSource object to current window
    winrt::check_hresult(interop->AttachToWindow(hwnd));

    // Get the new child window's hwnd
    HWND hWndXamlIsland = nullptr;
    winrt::check_hresult(interop->get_WindowHandle(&hWndXamlIsland));

    SetWindowPos(hWndXamlIsland, nullptr, 0, 0, createStruct->cx, createStruct->cy, SWP_SHOWWINDOW);

    return 0;
  }

  LRESULT OnDestroy() {
    PostQuitMessage(0);
    return 0;
  }

  LRESULT OnWindowPosChanged(HWND hwnd, const WINDOWPOS *windowPosition) {
    auto interop = m_desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
    HWND interopHwnd;
    winrt::check_hresult(interop->get_WindowHandle(&interopHwnd));

    MoveWindow(interopHwnd, 0, 0, windowPosition->cx, windowPosition->cy, TRUE);

    return 0;
  }

  /// Message handler for about box.
  static INT_PTR CALLBACK About(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    switch (message) {
      case WM_INITDIALOG:
        return TRUE;

      case WM_COMMAND:
        if (LOWORD(wparam) == IDOK || LOWORD(wparam) == IDCANCEL) {
          EndDialog(hwnd, LOWORD(wparam));
          return TRUE;
        }
        break;
    }

    return FALSE;
  }

  static constexpr std::wstring_view g_bundleFiles[] = {
      LR"(Samples\rntester)",     LR"(Samples\accessible)",
      LR"(Samples\callbackTest)", LR"(Samples\calculator)",
      LR"(Samples\click)",        LR"(Samples\customViewManager)",
      LR"(Samples\control)",      LR"(Samples\flexbox)",
      LR"(Samples\focusTest)",    LR"(Samples\geosample)",
      LR"(Samples\image)",        LR"(Samples\index)",
      LR"(Samples\mouse)",        LR"(Samples\scrollViewSnapSample)",
      LR"(Samples\simple)",       LR"(Samples\text)",
      LR"(Samples\textinput)",    LR"(Samples\ticTacToe)",
      LR"(Samples\view)",
  };

  static INT_PTR CALLBACK Bundle(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    switch (message) {
      case WM_INITDIALOG: {
        HWND hwndListBox = GetDlgItem(hwnd, IDC_JSBUNDLELIST);
        for (int i = 0; i < _countof(g_bundleFiles); i++) {
          SendMessage(hwndListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(g_bundleFiles[i].data()));
        }
        return TRUE;
      }
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDOK: {
            HWND hwndListBox = GetDlgItem(hwnd, IDC_JSBUNDLELIST);
            int selectedItem = static_cast<int>(SendMessage(hwndListBox, LB_GETCURSEL, 0, 0));
            if (0 <= selectedItem && selectedItem < _countof(g_bundleFiles)) {
              auto self = GetFromWindow(GetParent(hwnd));
              self->m_bundleFile = g_bundleFiles[selectedItem];
            }
          }
            [[fallthrough]];
          case IDCANCEL:
            EndDialog(hwnd, LOWORD(wparam));
            return TRUE;
        }
        break;
    }

    return FALSE;
  }

  static INT_PTR CALLBACK Settings(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    switch (message) {
      case WM_INITDIALOG: {
        auto boolToCheck = [](bool b) { return b ? BST_CHECKED : BST_UNCHECKED; };
        auto self = reinterpret_cast<WindowData *>(lparam);
        CheckDlgButton(hwnd, IDC_WEBDEBUGGER, boolToCheck(self->m_useWebDebugger));
        CheckDlgButton(hwnd, IDC_LIVERELOAD, boolToCheck(self->m_liveReloadEnabled));
        CheckDlgButton(hwnd, IDC_REUSEINSTANCE, boolToCheck(self->m_reuseInstance));
        return TRUE;
      }
      case WM_COMMAND: {
        switch (LOWORD(wparam)) {
          case IDOK: {
            auto self = GetFromWindow(GetParent(hwnd));
            self->m_useWebDebugger = IsDlgButtonChecked(hwnd, IDC_WEBDEBUGGER) == BST_CHECKED;
            self->m_liveReloadEnabled = IsDlgButtonChecked(hwnd, IDC_LIVERELOAD) == BST_CHECKED;
            self->m_reuseInstance = IsDlgButtonChecked(hwnd, IDC_REUSEINSTANCE) == BST_CHECKED;
          }
            [[fallthrough]];
          case IDCANCEL:
            EndDialog(hwnd, LOWORD(wparam));
            return true;
        }
        break;
      }
    }

    return FALSE;
  }
};

extern "C" IMAGE_DOS_HEADER __ImageBase;
HINSTANCE WindowData::s_instance = reinterpret_cast<HINSTANCE>(&__ImageBase);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  switch (message) {
    case WM_CREATE: {
      return WindowData::GetFromWindow(hwnd)->OnCreate(hwnd, reinterpret_cast<LPCREATESTRUCT>(lparam));
    }
    case WM_COMMAND: {
      return WindowData::GetFromWindow(hwnd)->OnCommand(
          hwnd, LOWORD(wparam), reinterpret_cast<HWND>(lparam), HIWORD(wparam));
    }
    case WM_DESTROY: {
      return WindowData::GetFromWindow(hwnd)->OnDestroy();
    }
    case WM_NCCREATE: {
      auto cs = reinterpret_cast<CREATESTRUCT *>(lparam);
      auto windowData = static_cast<WindowData *>(cs->lpCreateParams);
      WINRT_ASSERT(windowData);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowData));
      break;
    }
    case WM_WINDOWPOSCHANGED: {
      return WindowData::GetFromWindow(hwnd)->OnWindowPosChanged(hwnd, reinterpret_cast<const WINDOWPOS *>(lparam));
    }
  }

  return DefWindowProc(hwnd, message, wparam, lparam);
}

_Use_decl_annotations_ int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, PSTR commandLine, int showCmd) {
  constexpr PCWSTR appName = L"ReactNative Playground (Win32)";
  constexpr PCWSTR windowClassName = L"MS_REACTNATIVE_PLAYGROUND_WIN32";

  winrt::init_apartment(winrt::apartment_type::single_threaded);

  WUXH::DesktopWindowXamlSource desktopXamlSource;

  WNDCLASSEXW wcex = {};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = &WndProc;
  wcex.cbClsExtra = DLGWINDOWEXTRA;
  wcex.cbWndExtra = 0;
  wcex.hInstance = instance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLAYGROUND_WIN32);
  wcex.lpszClassName = windowClassName;
  ATOM classId = RegisterClassEx(&wcex);
  WINRT_VERIFY(classId);
  winrt::check_win32(!classId);

  auto windowData = std::make_unique<WindowData>(desktopXamlSource);

  HWND hwnd = CreateWindow(
      windowClassName,
      appName,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      800,
      600,
      nullptr,
      nullptr,
      instance,
      windowData.get());

  WINRT_VERIFY(hwnd);
  winrt::check_win32(!hwnd);

  ShowWindow(hwnd, showCmd);
  UpdateWindow(hwnd);
  SetFocus(hwnd);

  HACCEL hAccelTable = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_PLAYGROUND_WIN32));

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    auto xamlSourceNative2 = desktopXamlSource.as<IDesktopWindowXamlSourceNative2>();
    BOOL xamlSourceProcessedMessage = FALSE;
    winrt::check_hresult(xamlSourceNative2->PreTranslateMessage(&msg, &xamlSourceProcessedMessage));
    if (xamlSourceProcessedMessage) {
      continue;
    }

    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  windowData.reset();

  winrt::uninit_apartment();

  return (int)msg.wParam;
}
