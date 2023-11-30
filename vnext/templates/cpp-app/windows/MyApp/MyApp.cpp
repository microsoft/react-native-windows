// {{ name }}.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "{{ name }}.h"

#include "../../node_modules/react-native-windows/codegen/NativeDeviceInfoSpec.g.h"

#include <DispatcherQueue.h>
#include <UIAutomation.h>

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>

#include "NativeModules.h"
#include "ReactPropertyBag.h"

constexpr size_t MAX_LOADSTRING = 100;

// Work around crash in DeviceInfo when running outside of XAML environment
// TODO rework built-in DeviceInfo to allow it to be driven without use of HWNDs or XamlApps
// Issue Tracking #11414
REACT_MODULE(DeviceInfo)
struct DeviceInfo {
  using ModuleSpec = Microsoft::ReactNativeSpecs::DeviceInfoSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept {
    m_context = reactContext;
  }

  REACT_GET_CONSTANTS(GetConstants)
  Microsoft::ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants GetConstants() noexcept {
    Microsoft::ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants constants;
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
    AddAttributedModules(packageBuilder, true);
  }
};

// Global Variables:
WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

winrt::Windows::System::DispatcherQueueController g_dispatcherQueueController{nullptr};
winrt::Windows::UI::Composition::Compositor g_compositor{nullptr};

constexpr auto WindowDataProperty = L"WindowData";
constexpr PCWSTR c_windowClassName = L"MS_REACTNATIVE_RNTESTER_COMPOSITION";
constexpr PCWSTR appName = L"{{ mainComponentName }}";

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int RunRNTester(int showCmd);

struct WindowData {
  static HINSTANCE s_instance;
  static constexpr uint16_t defaultDebuggerPort{9229};

  bool m_windowInited{false};
  winrt::Microsoft::ReactNative::CompositionHwndHost m_CompositionHwndHost{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};

#if BUNDLE
  std::wstring m_bundleFile = L"index.windows";
  bool m_useWebDebugger{false};
  bool m_fastRefreshEnabled{false};
#else
  std::wstring m_bundleFile = L"index";
  bool m_useWebDebugger{false};
  bool m_fastRefreshEnabled{true};
#endif

  bool m_useDirectDebugger{false};
  bool m_breakOnNextLine{false};
  uint16_t m_debuggerPort{defaultDebuggerPort};
  xaml::ElementTheme m_theme{xaml::ElementTheme::Default};

  WindowData(const winrt::Microsoft::ReactNative::CompositionHwndHost &compHost) : m_CompositionHwndHost(compHost) {
    winrt::Microsoft::ReactNative::Composition::CompositionUIService::SetCompositionContext(
        InstanceSettings().Properties(),
        winrt::Microsoft::ReactNative::Composition::WindowsCompositionContextHelper::CreateContext(g_compositor));
  }

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

  LRESULT RenderApp(HWND hwnd) {
    WCHAR workingDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, workingDir);

    auto host = Host();
    // Disable until we have a 3rd party story for custom components
    // RegisterAutolinkedNativeModulePackages(host.PackageProviders()); // Includes any
    // autolinked modules

    host.InstanceSettings().JavaScriptBundleFile(m_bundleFile);

    host.InstanceSettings().UseWebDebugger(m_useWebDebugger);
    host.InstanceSettings().UseDirectDebugger(m_useDirectDebugger);
    host.InstanceSettings().BundleRootPath(std::wstring(L"file:").append(workingDir).append(L"\\Bundle\\").c_str());
    host.InstanceSettings().DebuggerBreakOnNextLine(m_breakOnNextLine);
    host.InstanceSettings().UseFastRefresh(m_fastRefreshEnabled);
    host.InstanceSettings().DebuggerPort(m_debuggerPort);
    host.InstanceSettings().UseDeveloperSupport(true);

    host.PackageProviders().Append(winrt::make<CompReactPackageProvider>());
    winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
        host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

    // Nudge the ReactNativeHost to create the instance and wrapping context
    host.ReloadInstance();

    winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
    viewOptions.ComponentName(appName);
    m_CompositionHwndHost.ReactViewHost(
        winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));

    auto windowData = WindowData::GetFromWindow(hwnd);
    if (!windowData->m_windowInited) {
      m_CompositionHwndHost.Initialize((uint64_t)(hwnd));
      windowData->m_windowInited = true;
    }
    return 0;
  }

  LRESULT TranslateMessage(UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    if (m_CompositionHwndHost) {
      return static_cast<LRESULT>(m_CompositionHwndHost.TranslateMessage(message, wparam, lparam));
    }
    return 0;
  }
};

extern "C" IMAGE_DOS_HEADER __ImageBase;
HINSTANCE WindowData::s_instance = reinterpret_cast<HINSTANCE>(&__ImageBase);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  auto windowData = WindowData::GetFromWindow(hWnd);
  if (windowData) {
    auto result = WindowData::GetFromWindow(hWnd)->TranslateMessage(message, wParam, lParam);
    if (result)
      return result;
  }

  switch (message) {
    case WM_DESTROY: {
      delete WindowData::GetFromWindow(hWnd);
      SetProp(hWnd, WindowDataProperty, 0);
      PostQuitMessage(0);
      return 0;
    }
    case WM_NCCREATE: {
      auto cs = reinterpret_cast<CREATESTRUCT *>(lParam);
      auto windowData = static_cast<WindowData *>(cs->lpCreateParams);
      WINRT_ASSERT(windowData);
      SetProp(hWnd, WindowDataProperty, reinterpret_cast<HANDLE>(windowData));
      break;
    }
    case WM_GETOBJECT: {
      {
        auto windowData = WindowData::GetFromWindow(hWnd);
        if (windowData == nullptr || !windowData->m_windowInited)
          break;

        auto hwndHost = windowData->m_CompositionHwndHost;
        winrt::com_ptr<IRawElementProviderSimple> spReps;
        if (!hwndHost.UiaProvider().try_as(spReps)) {
          break;
        }
        LRESULT lResult = UiaReturnRawElementProvider(hWnd, wParam, lParam, spReps.get());
        return lResult;
      }
    }
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

int RunRNTester(int showCmd) {
  auto windowData = std::make_unique<WindowData>(winrt::Microsoft::ReactNative::CompositionHwndHost());
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

  windowData.release();

  ShowWindow(hwnd, showCmd);
  UpdateWindow(hwnd);
  SetFocus(hwnd);
  WindowData::GetFromWindow(hwnd)->RenderApp(hwnd);

  HACCEL hAccelTable = LoadAccelerators(WindowData::s_instance, MAKEINTRESOURCE(IDC_RNTESTER_COMPOSITION));

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return static_cast<int>(msg.wParam);
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
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RNTESTER_COMPOSITION);
  wcex.lpszClassName = c_windowClassName;
  wcex.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_ICON1));
  ATOM classId = RegisterClassEx(&wcex);
  WINRT_VERIFY(classId);
  winrt::check_win32(!classId);

  DispatcherQueueOptions options{
      sizeof(DispatcherQueueOptions), /* dwSize */
      DQTYPE_THREAD_CURRENT, /* threadType */
      DQTAT_COM_ASTA /* apartmentType */
  };

  // Need to have a Dispatcher on the current thread to be able to create a Compositor
  winrt::check_hresult(CreateDispatcherQueueController(
      options,
      reinterpret_cast<ABI::Windows::System::IDispatcherQueueController **>(
          winrt::put_abi(g_dispatcherQueueController))));

  g_compositor = winrt::Windows::UI::Composition::Compositor();
  return RunRNTester(showCmd);
}
