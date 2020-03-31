#include "pch.h"
#include "resource.h"

#include <shlobj.h>
#include <shobjidl.h>
#include <windows.h>

#include <ReactUWP/IXamlRootView.h>
#include <ReactUWP/InstanceFactory.h>
#include <ReactUWP/ReactUwp.h>
#include <ReactWindowsCore/AsyncStorageModuleWin32Config.h>

#include <Unicode.h>

#include <IReactInstance.h>
#include <ViewManager.h>
#include <memory>

#include <filesystem>

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <Windows.UI.Xaml.Hosting.DesktopWindowXamlSource.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>

#pragma pop_macro("GetCurrentTime")

namespace {

std::string GetAsyncLocalStorageDBPath() {
  winrt::com_array<WCHAR> localAppData;
  winrt::check_hresult(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, put_abi(localAppData)));

  std::filesystem::path asyncLocalStorageDBDir{localAppData.data()};
  asyncLocalStorageDBDir /= LR"(Microsoft\React Native Playground (Win32))";

  if (!CreateDirectoryW(asyncLocalStorageDBDir.wstring().data(), nullptr)) {
    if (::GetLastError() != ERROR_ALREADY_EXISTS)
      winrt::throw_last_error();
  }

  auto asyncLocalStoragePath = asyncLocalStorageDBDir / L"AsyncStorage.sqlite3";
  return Microsoft::Common::Unicode::Utf16ToUtf8(asyncLocalStoragePath.wstring());
}

} // namespace

namespace WUX = winrt::Windows::UI::Xaml;
namespace WUXC = WUX::Controls;
namespace WUXH = WUX::Hosting;

struct PlaygroundNativeModuleProvider final : facebook::react::NativeModuleProvider {
  virtual std::vector<facebook::react::NativeModuleDescription> GetModules(
      const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread) override {
    std::vector<facebook::react::NativeModuleDescription> modules;
    return modules;
  }
};

class PlaygroundViewManagerProvider final : public react::uwp::ViewManagerProvider {
 public:
  virtual std::vector<react::uwp::NativeViewManager> GetViewManagers(
      const std::shared_ptr<react::uwp::IReactInstance> &instance) override {
    std::vector<react::uwp::NativeViewManager> viewManagers;

    return viewManagers;
  }
};

struct HwndReactInstanceCreator : ::react::uwp::IReactInstanceCreator {
  HwndReactInstanceCreator(HWND hwnd) {
    m_hwnd = hwnd;
  }

  void detach() {
    m_hwnd = nullptr;
  }

  std::shared_ptr<::react::uwp::IReactInstance> getInstance() override;
  void markAsNeedsReload() override;
  void persistUseWebDebugger(bool useWebDebugger) override;
  void persistUseLiveReload(bool useLiveReload) override;
  void persistUseDirectDebugger(bool useDirectDebugger) override;
  void persistBreakOnNextLine(bool breakOnNextLine) override;

 private:
  HWND m_hwnd;
};

struct WindowData {
  static HINSTANCE s_instance;
  static constexpr uint16_t defaultDebuggerPort = 9229;

  std::wstring m_bundleFile;
  WUXH::DesktopWindowXamlSource m_desktopWindowXamlSource;
  std::shared_ptr<react::uwp::IReactInstance> m_instance;
  std::shared_ptr<react::uwp::IXamlRootView> m_rootView;
  std::shared_ptr<HwndReactInstanceCreator> m_instanceCreator;

  bool m_useWebDebugger{true};
  bool m_liveReloadEnabled{true};
  bool m_reuseInstance{true};
  bool m_useDirectDebugger{false};
  bool m_breakOnNextLine{false};
  uint16_t m_debuggerPort{defaultDebuggerPort};

  react::uwp::JSIEngine m_jsEngine{react::uwp::JSIEngine::Chakra};

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
          settings.DebuggerBreakOnNextLine = m_breakOnNextLine;
          settings.UseDirectDebugger = m_useDirectDebugger;
          settings.DebuggerPort = m_debuggerPort;
          settings.jsiEngine = m_jsEngine;

          settings.EnableDeveloperMenu = true;

          settings.LoggingCallback = [](facebook::react::RCTLogLevel logLevel, const char *message) {
            OutputDebugStringA("In LoggingCallback");
            OutputDebugStringA(message);
          };

          m_instance->Start(m_instance, settings);
          m_instance->loadBundle(Microsoft::Common::Unicode::Utf16ToUtf8(m_bundleFile));

          folly::dynamic initialProps = folly::dynamic::object();

          // Retrieve ABI pointer from C++/CX pointer
          auto rootElement = m_desktopWindowXamlSource.Content().as<WUX::FrameworkElement>();

          PCWSTR appName = (m_bundleFile == LR"(Samples\rntester)") ? L"RNTesterApp" : L"Bootstrap";

          // Create the root view
          m_rootView = react::uwp::CreateReactRootView(rootElement, appName, m_instanceCreator);

          m_rootView->SetInitialProps(std::move(initialProps));
          m_rootView->SetInstanceCreator(m_instanceCreator);
          m_rootView->AttachRoot();
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

    try {
      m_instanceCreator = std::make_shared<HwndReactInstanceCreator>(hwnd);
    } catch (...) {
      return -1;
    }

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
        CheckDlgButton(hwnd, IDC_DIRECTDEBUGGER, boolToCheck(self->m_useDirectDebugger));
        CheckDlgButton(hwnd, IDC_BREAKONNEXTLINE, boolToCheck(self->m_breakOnNextLine));

        auto portEditControl = GetDlgItem(hwnd, IDC_DEBUGGERPORT);
        SetWindowTextW(portEditControl, std::to_wstring(self->m_debuggerPort).c_str());
        SendMessageW(portEditControl, (UINT)EM_SETLIMITTEXT, (WPARAM)5, (LPARAM)0);

        auto cmbEngines = GetDlgItem(hwnd, IDC_JSENGINE);
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("Chakra"));
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("Hermes"));
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("V8"));
        SendMessageW(cmbEngines, CB_SETCURSEL, (WPARAM) static_cast<int32_t>(self->m_jsEngine), (LPARAM)0);

        return TRUE;
      }
      case WM_COMMAND: {
        switch (LOWORD(wparam)) {
          case IDOK: {
            auto self = GetFromWindow(GetParent(hwnd));
            self->m_useWebDebugger = IsDlgButtonChecked(hwnd, IDC_WEBDEBUGGER) == BST_CHECKED;
            self->m_liveReloadEnabled = IsDlgButtonChecked(hwnd, IDC_LIVERELOAD) == BST_CHECKED;
            self->m_reuseInstance = IsDlgButtonChecked(hwnd, IDC_REUSEINSTANCE) == BST_CHECKED;
            self->m_useDirectDebugger = IsDlgButtonChecked(hwnd, IDC_DIRECTDEBUGGER) == BST_CHECKED;
            self->m_breakOnNextLine = IsDlgButtonChecked(hwnd, IDC_BREAKONNEXTLINE) == BST_CHECKED;

            WCHAR buffer[6] = {};
            auto portEditControl = GetDlgItem(hwnd, IDC_DEBUGGERPORT);
            GetWindowTextW(portEditControl, buffer, ARRAYSIZE(buffer));

            try {
              auto port = std::stoi(buffer);
              if (port > UINT16_MAX)
                port = defaultDebuggerPort;
              self->m_debuggerPort = port;
            } catch (const std::out_of_range &) {
              self->m_debuggerPort = defaultDebuggerPort;
            } catch (const std::invalid_argument &) {
              // Don't update the debugger port if the new value can't be parsed
              // (E.g. includes letters or symbols).
            }

            auto cmbEngines = GetDlgItem(hwnd, IDC_JSENGINE);
            int itemIndex = (int)SendMessageW(cmbEngines, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            self->m_jsEngine = static_cast<react::uwp::JSIEngine>(itemIndex);
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

std::shared_ptr<::react::uwp::IReactInstance> HwndReactInstanceCreator::getInstance() {
  if (!m_hwnd)
    return nullptr;

  return WindowData::GetFromWindow(m_hwnd)->m_instance;
}

void HwndReactInstanceCreator::markAsNeedsReload() {
  if (!m_hwnd)
    return;

  auto instance = WindowData::GetFromWindow(m_hwnd)->m_instance;
  if (instance)
    instance->SetAsNeedsReload();
}

void HwndReactInstanceCreator::persistUseWebDebugger(bool useWebDebugger) {
  if (!m_hwnd)
    return;

  WindowData::GetFromWindow(m_hwnd)->m_useWebDebugger = useWebDebugger;
}

void HwndReactInstanceCreator::persistUseLiveReload(bool useLiveReload) {
  if (!m_hwnd)
    return;

  WindowData::GetFromWindow(m_hwnd)->m_liveReloadEnabled = useLiveReload;
}

void HwndReactInstanceCreator::persistUseDirectDebugger(bool useDirectDebugger) {
  if (!m_hwnd)
    return;

  WindowData::GetFromWindow(m_hwnd)->m_useDirectDebugger = useDirectDebugger;
}

void HwndReactInstanceCreator::persistBreakOnNextLine(bool breakOnNextLine) {
  if (!m_hwnd)
    return;

  WindowData::GetFromWindow(m_hwnd)->m_breakOnNextLine = breakOnNextLine;
}

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
      delete WindowData::GetFromWindow(hwnd);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
      PostQuitMessage(0);
      return 0;
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
  constexpr PCWSTR appName = L"React Native Playground (Win32)";
  constexpr PCWSTR windowClassName = L"MS_REACTNATIVE_PLAYGROUND_WIN32";

  winrt::init_apartment(winrt::apartment_type::single_threaded);

  react::windows::SetAsyncStorageDBPath(GetAsyncLocalStorageDBPath());

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

  auto xamlContent = WUXC::Grid();
  desktopXamlSource.Content(xamlContent);

  HWND hwnd = CreateWindow(
      windowClassName,
      appName,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      nullptr,
      nullptr,
      instance,
      windowData.get());

  WINRT_VERIFY(hwnd);
  winrt::check_win32(!hwnd);
  windowData.release();

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

    if (!TranslateAccelerator(hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  winrt::uninit_apartment();

  return (int)msg.wParam;
}
