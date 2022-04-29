#include "pch.h"
#include "resource.h"

#include <UI.Xaml.Hosting.DesktopWindowXamlSource.h>

#include <shlobj.h>
#include <shobjidl.h>
#include <windows.h>
#include <windowsx.h>

#include <filesystem>
#include <memory>
#include <thread>

#include <winrt/Microsoft.Toolkit.Win32.UI.XamlHost.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.XamlTypeInfo.h>

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <DesktopWindowBridge.h>

#include "AutolinkedNativeModules.g.h"

#include <CppWinRTIncludes.h>
#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Hosting.h>
#include <UI.Xaml.Markup.h>
#include <UI.Xaml.Media.h>
#include <winrt/Windows.Foundation.Collections.h>
#pragma pop_macro("GetCurrentTime")

#ifndef USE_WINUI3
#else
#include <winrt/Microsoft.UI.h>
#endif

#ifdef USE_WINCOMP
#include "../../../../vnext/codegen/NativeDeviceInfoSpec.g.h"
#include "../../../../vnext/codegen/NativeLogBoxSpec.g.h"
#include <DispatcherQueue.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include "NativeModules.h"
#include "ReactPropertyBag.h"
#endif // USE_WINCOMP

namespace controls = xaml::Controls;
namespace hosting = xaml::Hosting;

#ifdef USE_WINCOMP
// Work around crash in DeviceInfo when running outside of XAML environment
REACT_MODULE(DeviceInfo)
struct DeviceInfo {
  using ModuleSpec = Microsoft::ReactNativeSpecs::DeviceInfoSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept {
    m_context = reactContext;
  }

  REACT_GET_CONSTANTS(GetConstants)
  Microsoft::ReactNativeSpecs::DeviceInfoSpec_Constants GetConstants() noexcept {
    Microsoft::ReactNativeSpecs::DeviceInfoSpec_Constants constants;
    Microsoft::ReactNativeSpecs::DeviceInfoSpec_DisplayMetrics screenDisplayMetrics;
    screenDisplayMetrics.fontScale = 1;
    screenDisplayMetrics.height = 1024;
    screenDisplayMetrics.width = 1024;
    screenDisplayMetrics.scale = 1;
    constants.Dimensions.screen = screenDisplayMetrics;
    constants.Dimensions.window = screenDisplayMetrics;
    return constants;
  }

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

// Have to use TurboModules to override built in modules.. so the standard attributed package provider doesn't work.
struct CompReactPackageProvider
    : winrt::implements<CompReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    auto experimentalPackageBuilder =
        packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderExperimental>();
    experimentalPackageBuilder.AddTurboModule(
        L"DeviceInfo", winrt::Microsoft::ReactNative::MakeModuleProvider<DeviceInfo>());
  }
};

winrt::Windows::System::DispatcherQueueController g_dispatcherQueueController{nullptr};
winrt::Windows::UI::Composition::Compositor g_compositor{nullptr};
#endif // USE_WINCOMP

constexpr auto WindowDataProperty = L"WindowData";

int RunPlayground(int showCmd, bool useWebDebugger);

HWND GetXamlIslandHwnd(const hosting::DesktopWindowXamlSource &dwxs) {
  auto interop = dwxs.as<IDesktopWindowXamlSourceNative>();
  // Get the new child window's hwnd
  HWND hWndXamlIsland = nullptr;
  winrt::check_hresult(interop->get_WindowHandle(&hWndXamlIsland));
  return hWndXamlIsland;
}

struct WindowData {
  static HINSTANCE s_instance;
  static constexpr uint16_t defaultDebuggerPort = 9229;

  std::wstring m_bundleFile;
#ifdef USE_WINCOMP
  winrt::Microsoft::ReactNative::CompHwndHost m_compHwndHost{nullptr};
#else
  hosting::DesktopWindowXamlSource m_desktopWindowXamlSource{nullptr};
  winrt::Microsoft::ReactNative::ReactRootView m_reactRootView{nullptr};
#endif
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};

  bool m_useWebDebugger{false};
  bool m_fastRefreshEnabled{true};
  bool m_useDirectDebugger{false};
  bool m_breakOnNextLine{false};
  uint16_t m_debuggerPort{defaultDebuggerPort};
  xaml::ElementTheme m_theme{xaml::ElementTheme::Default};

#ifdef USE_WINCOMP
  WindowData(const winrt::Microsoft::ReactNative::CompHwndHost &compHost) : m_compHwndHost(compHost) {}
#else
  WindowData(const hosting::DesktopWindowXamlSource &desktopWindowXamlSource)
      : m_desktopWindowXamlSource(desktopWindowXamlSource) {}
#endif

  static WindowData *GetFromWindow(HWND hwnd) {
    auto data = reinterpret_cast<WindowData *>(GetProp(hwnd, WindowDataProperty));
    return data;
  }

  winrt::Microsoft::ReactNative::ReactNativeHost Host() noexcept {
    if (!m_host) {
      m_host = winrt::Microsoft::ReactNative::ReactNativeHost();
      m_host.InstanceSettings(InstanceSettings());
    }

    return m_host;
  }
  winrt::Microsoft::ReactNative::ReactInstanceSettings InstanceSettings() noexcept {
    if (!m_instanceSettings) {
      m_instanceSettings = winrt::Microsoft::ReactNative::ReactInstanceSettings();
    }

    return m_instanceSettings;
  }

  LRESULT OnCommand(HWND hwnd, int id, HWND /* hwndCtl*/, UINT) {
    switch (id) {
      case IDM_OPENJSFILE: {
        DialogBox(s_instance, MAKEINTRESOURCE(IDD_OPENJSBUNDLEBOX), hwnd, &Bundle);

        if (!m_bundleFile.empty()) {
          PCWSTR appName = (m_bundleFile == LR"(Samples\rntester)") ? L"RNTesterApp" : L"Bootstrap";

          WCHAR workingDir[MAX_PATH];
          GetCurrentDirectory(MAX_PATH, workingDir);

          auto host = Host();
          RegisterAutolinkedNativeModulePackages(host.PackageProviders()); // Includes any autolinked modules

          host.InstanceSettings().JavaScriptBundleFile(m_bundleFile);

          host.InstanceSettings().UseWebDebugger(m_useWebDebugger);
          host.InstanceSettings().UseDirectDebugger(m_useDirectDebugger);
          host.InstanceSettings().BundleRootPath(
              std::wstring(L"file:").append(workingDir).append(L"\\Bundle\\").c_str());
          host.InstanceSettings().DebuggerBreakOnNextLine(m_breakOnNextLine);
          host.InstanceSettings().UseFastRefresh(m_fastRefreshEnabled);
          host.InstanceSettings().DebuggerPort(m_debuggerPort);
          host.InstanceSettings().UseDeveloperSupport(true);

#ifdef USE_WINCOMP
          host.PackageProviders().Append(winrt::make<CompReactPackageProvider>());
          winrt::Microsoft::ReactNative::ReactPropertyBag(host.InstanceSettings().Properties())
              .Set(
                  winrt::Microsoft::ReactNative::ReactPropertyId<uint64_t>(L"RootHwndForDevUI"),
                  reinterpret_cast<uint64_t>(hwnd));
#else
          auto rootElement = m_desktopWindowXamlSource.Content().as<controls::Panel>();
          winrt::Microsoft::ReactNative::XamlUIService::SetXamlRoot(
              host.InstanceSettings().Properties(), rootElement.XamlRoot());
          winrt::Microsoft::ReactNative::XamlUIService::SetAccessibleRoot(
              host.InstanceSettings().Properties(), rootElement);
          rootElement.SetValue(
              winrt::Windows::UI::Xaml::Automation::AutomationProperties::LandmarkTypeProperty(),
              winrt::box_value(80002));

#ifdef USE_WINUI3
          const auto islandWindow = (uint64_t)GetXamlIslandHwnd(m_desktopWindowXamlSource);
          winrt::Microsoft::ReactNative::XamlUIService::SetIslandWindowHandle(
              host.InstanceSettings().Properties(), islandWindow);
#endif // USE_WINUI3

#endif

          // Nudge the ReactNativeHost to create the instance and wrapping context
          host.ReloadInstance();

#ifdef USE_WINCOMP
          m_compHwndHost.ComponentName(appName);
          m_compHwndHost.ReactNativeHost(host);
#else
          m_reactRootView = winrt::Microsoft::ReactNative::ReactRootView();
          m_reactRootView.ComponentName(appName);
          m_reactRootView.ReactNativeHost(host);
          rootElement.Children().Clear();
          rootElement.Children().Append(m_reactRootView);
#endif
        }

        break;
      }
      case IDM_NEWWINDOW: {
        std::thread playgroundThread{([]() {
          // For subsequent RN windows do not use the web debugger by default,
          // since one instance can be connected to it at a time.
          RunPlayground(SW_SHOW, false);
        })};
        playgroundThread.detach();
        break;
      }
      case IDM_ABOUT:
        DialogBox(s_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, &About);
        break;
      case IDM_EXIT:
        PostQuitMessage(0);
        break;
      case IDM_REFRESH:
        Host().ReloadInstance();
        break;
      case IDM_SETTINGS:
        DialogBoxParam(s_instance, MAKEINTRESOURCE(IDD_SETTINGSBOX), hwnd, &Settings, reinterpret_cast<INT_PTR>(this));
        break;
    }

    return 0;
  }

#ifdef USE_WINCOMP
  LRESULT TranslateMessage(UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    if (m_compHwndHost) {
      return m_compHwndHost.TranslateMessage(message, wparam, lparam);
    }
    return 0;
  }
#endif

  LRESULT OnCreate(HWND hwnd, LPCREATESTRUCT createStruct) {
#ifdef USE_WINCOMP
    if (m_compHwndHost) {
      m_compHwndHost.Compositor(g_compositor);
      m_compHwndHost.Initialize((uint64_t)hwnd);
    }
#else
    auto interop = m_desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
    // Parent the DesktopWindowXamlSource object to current window
    winrt::check_hresult(interop->AttachToWindow(hwnd));

    auto hWndXamlIsland = GetXamlIslandHwnd(m_desktopWindowXamlSource);
    SetWindowPos(hWndXamlIsland, nullptr, 0, 0, createStruct->cx, createStruct->cy, SWP_SHOWWINDOW);
#endif
    return 0;
  }

#ifndef USE_WINCOMP
  LRESULT OnWindowPosChanged(HWND /* hwnd */, const WINDOWPOS *windowPosition) {
    auto interop = m_desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
    HWND interopHwnd;
    winrt::check_hresult(interop->get_WindowHandle(&interopHwnd));

    constexpr int logBoxHeight = 100;
    constexpr int scrollbarWidth = 24;
    MoveWindow(
        interopHwnd,
        0,
        0,
        windowPosition->cx > scrollbarWidth ? windowPosition->cx - scrollbarWidth : windowPosition->cx,
        windowPosition->cy > logBoxHeight ? windowPosition->cy - logBoxHeight : windowPosition->cy,
        TRUE);

    return 0;
  }
#endif

  /// Message handler for about box.
  static INT_PTR CALLBACK About(HWND hwnd, UINT message, WPARAM wparam, LPARAM /* lparam */) noexcept {
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

  static INT_PTR CALLBACK Bundle(HWND hwnd, UINT message, WPARAM wparam, LPARAM /*lparam*/) noexcept {
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
        CheckDlgButton(hwnd, IDC_FASTREFRESH, boolToCheck(self->m_fastRefreshEnabled));
        CheckDlgButton(hwnd, IDC_DIRECTDEBUGGER, boolToCheck(self->m_useDirectDebugger));
        CheckDlgButton(hwnd, IDC_BREAKONNEXTLINE, boolToCheck(self->m_breakOnNextLine));

        auto portEditControl = GetDlgItem(hwnd, IDC_DEBUGGERPORT);
        SetWindowTextW(portEditControl, std::to_wstring(self->m_debuggerPort).c_str());
        SendMessageW(portEditControl, (UINT)EM_SETLIMITTEXT, (WPARAM)5, (LPARAM)0);

        auto cmbEngines = GetDlgItem(hwnd, IDC_JSENGINE);
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("Chakra"));
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("Hermes"));
        SendMessageW(cmbEngines, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)TEXT("V8"));
        // SendMessageW(cmbEngines, CB_SETCURSEL, (WPARAM) static_cast<int32_t>(self->m_jsEngine), (LPARAM)0);

        auto cmbTheme = GetDlgItem(hwnd, IDC_THEME);
        SendMessageW(cmbTheme, CB_ADDSTRING, 0, (LPARAM)L"Default");
        SendMessageW(cmbTheme, CB_ADDSTRING, 0, (LPARAM)L"Light");
        SendMessageW(cmbTheme, CB_ADDSTRING, 0, (LPARAM)L"Dark");
        ComboBox_SetCurSel(cmbTheme, static_cast<int>(self->m_theme));

        return TRUE;
      }
      case WM_COMMAND: {
        switch (LOWORD(wparam)) {
          case IDOK: {
            auto self = GetFromWindow(GetParent(hwnd));
            self->m_useWebDebugger = IsDlgButtonChecked(hwnd, IDC_WEBDEBUGGER) == BST_CHECKED;
            self->m_fastRefreshEnabled = IsDlgButtonChecked(hwnd, IDC_FASTREFRESH) == BST_CHECKED;
            self->m_useDirectDebugger = IsDlgButtonChecked(hwnd, IDC_DIRECTDEBUGGER) == BST_CHECKED;
            self->m_breakOnNextLine = IsDlgButtonChecked(hwnd, IDC_BREAKONNEXTLINE) == BST_CHECKED;

            auto themeComboBox = GetDlgItem(hwnd, IDC_THEME);
#ifndef USE_WINCOMP
            self->m_theme = static_cast<xaml::ElementTheme>(ComboBox_GetCurSel(themeComboBox));
            auto panel = self->m_desktopWindowXamlSource.Content().as<controls::Panel>();
            panel.RequestedTheme(self->m_theme);
#endif

            WCHAR buffer[6] = {};
            auto portEditControl = GetDlgItem(hwnd, IDC_DEBUGGERPORT);
            GetWindowTextW(portEditControl, buffer, ARRAYSIZE(buffer));

            try {
              auto port = std::stoi(buffer);
              if (port > UINT16_MAX)
                port = defaultDebuggerPort;
              self->m_debuggerPort = static_cast<uint16_t>(port);
            } catch (const std::out_of_range &) {
              self->m_debuggerPort = defaultDebuggerPort;
            } catch (const std::invalid_argument &) {
              // Don't update the debugger port if the new value can't be parsed
              // (E.g. includes letters or symbols).
            }

            // auto cmbEngines = GetDlgItem(hwnd, IDC_JSENGINE);
            // int itemIndex = (int)SendMessageW(cmbEngines, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            // self->m_jsEngine = static_cast<Microsoft::ReactNative::JSIEngine>(itemIndex);
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
#ifdef USE_WINUI3
  if (auto windowData = WindowData::GetFromWindow(hwnd)) {
    winrt::Microsoft::ReactNative::ReactNotificationService rns(windowData->InstanceSettings().Notifications());
    winrt::Microsoft::ReactNative::ForwardWindowMessage(rns, hwnd, message, wparam, lparam);
  }
#endif

#ifdef USE_WINCOMP
  auto windowData = WindowData::GetFromWindow(hwnd);
  if (windowData) {
    auto result = WindowData::GetFromWindow(hwnd)->TranslateMessage(message, wparam, lparam);
    if (result)
      return result;
  }
#endif

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
      SetProp(hwnd, WindowDataProperty, 0);
      PostQuitMessage(0);
      return 0;
    }
    case WM_NCCREATE: {
      auto cs = reinterpret_cast<CREATESTRUCT *>(lparam);
      auto windowData = static_cast<WindowData *>(cs->lpCreateParams);
      WINRT_ASSERT(windowData);
      SetProp(hwnd, WindowDataProperty, reinterpret_cast<HANDLE>(windowData));
      break;
    }
#ifdef USE_WINCOMP
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      // TODO: Add any drawing code that uses hdc here...
      EndPaint(hwnd, &ps);
    } break;
#else
    case WM_WINDOWPOSCHANGED: {
      return WindowData::GetFromWindow(hwnd)->OnWindowPosChanged(hwnd, reinterpret_cast<const WINDOWPOS *>(lparam));
    }
#endif
  }

  return DefWindowProc(hwnd, message, wparam, lparam);
}

constexpr PCWSTR c_windowClassName = L"MS_REACTNATIVE_PLAYGROUND_WIN32";

int RunPlayground(int showCmd, bool useWebDebugger) {
#ifdef USE_WINUI3
  constexpr PCWSTR appName = L"React Native Playground (Win32 WinUI3)";
#else
  constexpr PCWSTR appName = L"React Native Playground (Win32)";
#endif

#ifdef USE_WINCOMP
  auto windowData = std::make_unique<WindowData>(winrt::Microsoft::ReactNative::CompHwndHost());
#else
  winrt::init_apartment(winrt::apartment_type::single_threaded);

  auto winuiIXMP = winrt::Microsoft::UI::Xaml::XamlTypeInfo::XamlControlsXamlMetaDataProvider();

  auto xapp = winrt::Microsoft::Toolkit::Win32::UI::XamlHost::XamlApplication({winuiIXMP});

  winrt::Windows::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();

  xapp.Resources().MergedDictionaries().Append(winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources());

  hosting::DesktopWindowXamlSource desktopXamlSource;
  auto windowData = std::make_unique<WindowData>(desktopXamlSource);
  windowData->m_useWebDebugger = useWebDebugger;

  // We have to use a XAML string here to access the ThemeResource.
  // XAML Islands requires us to set the background color to handle theme changes.
  const winrt::hstring xamlString =
      LR"(
  <Grid
    xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'
    xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'
    Background='{ThemeResource ApplicationPageBackgroundThemeBrush}' />)";
  auto xamlContent = winrt::unbox_value<controls::Grid>(xaml::Markup::XamlReader::Load(xamlString));
  desktopXamlSource.Content(xamlContent);
#endif

  HWND hwnd = CreateWindow(
      c_windowClassName,
      appName,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      nullptr,
      nullptr,
      WindowData::s_instance,
      windowData.get());

  WINRT_VERIFY(hwnd);
  winrt::check_win32(!hwnd);

  windowData.release();

  ShowWindow(hwnd, showCmd);
  UpdateWindow(hwnd);
  SetFocus(hwnd);

  HACCEL hAccelTable = LoadAccelerators(WindowData::s_instance, MAKEINTRESOURCE(IDC_PLAYGROUND_WIN32));

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
#ifndef USE_WINCOMP
    auto xamlSourceNative2 = desktopXamlSource.as<IDesktopWindowXamlSourceNative2>();
    BOOL xamlSourceProcessedMessage = FALSE;
    winrt::check_hresult(xamlSourceNative2->PreTranslateMessage(&msg, &xamlSourceProcessedMessage));
    if (xamlSourceProcessedMessage) {
      continue;
    }
#endif // !USE_WINCOMP
    if (!TranslateAccelerator(hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

#ifndef USE_WINCOMP
  winrt::uninit_apartment();
#endif

  return (int)msg.wParam;
}

_Use_decl_annotations_ int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, PSTR /* commandLine */, int showCmd) {
  WNDCLASSEXW wcex = {};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = &WndProc;
  wcex.cbClsExtra = DLGWINDOWEXTRA;
  wcex.cbWndExtra = sizeof(WindowData *);
  wcex.hInstance = WindowData::s_instance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PLAYGROUND_WIN32);
  wcex.lpszClassName = c_windowClassName;
  wcex.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_ICON1));
  ATOM classId = RegisterClassEx(&wcex);
  WINRT_VERIFY(classId);
  winrt::check_win32(!classId);

#ifdef USE_WINCOMP
  DispatcherQueueOptions options{
      sizeof(DispatcherQueueOptions), /* dwSize */
      DQTYPE_THREAD_CURRENT, /* threadType */
      DQTAT_COM_ASTA /* apartmentType */
  };

  winrt::check_hresult(CreateDispatcherQueueController(
      options,
      reinterpret_cast<ABI::Windows::System::IDispatcherQueueController **>(
          winrt::put_abi(g_dispatcherQueueController))));

  g_compositor = winrt::Windows::UI::Composition::Compositor();
#endif
  return RunPlayground(showCmd, true);
}
