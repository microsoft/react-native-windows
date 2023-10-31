// RNTesterApp-Fabric.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "RNTesterApp-Fabric.h"

#include "../../../../vnext/codegen/NativeDeviceInfoSpec.g.h"
#include "winrt/AutomationChannel.h"

#include <DispatcherQueue.h>
#include <UIAutomation.h>
#include <combaseapi.h>
#include <unknwn.h>

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
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
HWND global_hwnd;

winrt::Windows::System::DispatcherQueueController g_dispatcherQueueController{nullptr};
winrt::Windows::UI::Composition::Compositor g_compositor{nullptr};
winrt::AutomationChannel::CommandHandler handler;
winrt::AutomationChannel::Server server{nullptr};

constexpr auto WindowDataProperty = L"WindowData";
constexpr PCWSTR c_windowClassName = L"MS_REACTNATIVE_RNTESTER_COMPOSITION";
constexpr PCWSTR appName = L"RNTesterApp";

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int RunRNTester(int showCmd);
winrt::Windows::Data::Json::JsonObject ListErrors(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Data::Json::JsonObject DumpVisualTree(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Foundation::IAsyncAction LoopServer(winrt::AutomationChannel::Server &server);

struct WindowData {
  static HINSTANCE s_instance;
  static constexpr uint16_t defaultDebuggerPort{9229};

  bool m_windowInited{false};
  winrt::Microsoft::ReactNative::CompositionHwndHost m_CompositionHwndHost{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};

#if BUNDLE
  bool m_useWebDebugger{false};
  bool m_fastRefreshEnabled{false};
#else
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

    host.InstanceSettings().JavaScriptBundleFile(L"index.windows");
    host.InstanceSettings().DebugBundlePath(L"index");
    host.InstanceSettings().UseWebDebugger(m_useWebDebugger);
    host.InstanceSettings().UseDirectDebugger(m_useDirectDebugger);
    host.InstanceSettings().BundleRootPath(std::wstring(L"file:").append(workingDir).append(L"\\Bundle\\").c_str());
    host.InstanceSettings().DebuggerBreakOnNextLine(m_breakOnNextLine);
    host.InstanceSettings().UseFastRefresh(m_fastRefreshEnabled);
    host.InstanceSettings().DebuggerPort(m_debuggerPort);
    host.InstanceSettings().UseDeveloperSupport(true);

    host.PackageProviders().Append(winrt::make<CompReactPackageProvider>());
    host.PackageProviders().Append(winrt::AutomationChannel::ReactPackageProvider());
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
      if (lParam == UiaRootObjectId) {
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
  global_hwnd = hwnd;

  windowData.release();

  ShowWindow(hwnd, showCmd);
  UpdateWindow(hwnd);
  SetFocus(hwnd);
  WindowData::GetFromWindow(hwnd)->RenderApp(hwnd);

  HACCEL hAccelTable = LoadAccelerators(WindowData::s_instance, MAKEINTRESOURCE(IDC_RNTESTER_COMPOSITION));

  // Set Up Servers for E2E Testing
  handler.BindOperation(L"DumpVisualTree", DumpVisualTree);
  handler.BindOperation(L"ListErrors", ListErrors);
  server = winrt::AutomationChannel::Server(handler);
  auto asyncAction = LoopServer(server);

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

winrt::Windows::Data::Json::JsonObject ListErrors(winrt::Windows::Data::Json::JsonValue payload) {
  winrt::Windows::Data::Json::JsonObject result;
  winrt::Windows::Data::Json::JsonArray jsonErrors;
  winrt::Windows::Data::Json::JsonArray jsonWarnings;
  // TODO: Add Error and Warnings
  result.Insert(L"errors", jsonErrors);
  result.Insert(L"warnings", jsonWarnings);
  return result;
}

winrt::Windows::Data::Json::JsonObject DumpUIATreeRecurse(IUIAutomationElement *pTarget, IUIAutomationTreeWalker *pWalker) {
  winrt::Windows::Data::Json::JsonObject result;
  BSTR automationId;
  CONTROLTYPEID controlType;
  BSTR helpText;
  BOOL isEnabled;
  BOOL isKeyboardFocusable;
  BSTR localizedControlType;
  BSTR name;

  pTarget->get_CurrentAutomationId(&automationId);
  pTarget->get_CurrentControlType(&controlType);
  pTarget->get_CurrentHelpText(&helpText);
  pTarget->get_CurrentIsEnabled(&isEnabled);
  pTarget->get_CurrentIsKeyboardFocusable(&isKeyboardFocusable);
  pTarget->get_CurrentLocalizedControlType(&localizedControlType);
  pTarget->get_CurrentName(&name);
  result.Insert(L"AutomationId", winrt::Windows::Data::Json::JsonValue::CreateStringValue(automationId));
  result.Insert(L"ControlType", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(controlType));
  result.Insert(L"HelpText", winrt::Windows::Data::Json::JsonValue::CreateStringValue(helpText));
  result.Insert(L"IsEnabled", winrt::Windows::Data::Json::JsonValue::CreateBooleanValue(isEnabled));
  result.Insert(L"IsKeyboardFocusable", winrt::Windows::Data::Json::JsonValue::CreateBooleanValue(isKeyboardFocusable));
  result.Insert(
      L"LocalizedControlType", winrt::Windows::Data::Json::JsonValue::CreateStringValue(localizedControlType));
  result.Insert(L"Name", winrt::Windows::Data::Json::JsonValue::CreateStringValue(name));

  IUIAutomationElement *pChild;
  IUIAutomationElement *pSibling;
  pWalker->GetFirstChildElement(pTarget, &pChild);
  winrt::Windows::Data::Json::JsonArray children;
  while (pChild != nullptr) {
    children.Append(DumpUIATreeRecurse(pChild, pWalker));
    pWalker->GetNextSiblingElement(pChild, &pSibling);
    pChild = pSibling;
    pSibling = nullptr;
  }
  if (children.Size() > 0) {
    result.Insert(L"Children", children);
  }
  return result;
}

winrt::Windows::Data::Json::JsonObject DumpUIATreeHelper(winrt::Windows::Data::Json::JsonObject payloadObj) {
  auto accessibilityId = payloadObj.GetNamedString(L"accessibilityId");

  winrt::Windows::Data::Json::JsonObject result;

  IUIAutomation *pAutomation;
  IUIAutomationElement *pRootElement;
  IUIAutomationTreeWalker *pWalker;

  CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAutomation));
  pAutomation->get_ContentViewWalker(&pWalker);
  pAutomation->ElementFromHandle(global_hwnd, &pRootElement);

  IUIAutomationElement *pTarget;
  IUIAutomationCondition *pCondition;
  VARIANT varAutomationId;
  VariantInit(&varAutomationId);

  varAutomationId.vt = VT_BSTR;
  varAutomationId.bstrVal = SysAllocString(accessibilityId.c_str());
  pAutomation->CreatePropertyCondition(UIA_AutomationIdPropertyId, varAutomationId, &pCondition);
  pRootElement->FindFirst(TreeScope_Descendants, pCondition, &pTarget);

  winrt::Windows::Data::Json::JsonObject uiaTree;
  if (pTarget != nullptr) {
    uiaTree = DumpUIATreeRecurse(pTarget, pWalker);
  }

  pWalker->Release();
  pRootElement->Release();
  pAutomation->Release();
  pCondition->Release();

  return uiaTree;
}

winrt::Windows::Data::Json::JsonObject PrintVisualTree(winrt::Windows::UI::Composition::Visual node){
  winrt::Windows::Data::Json::JsonObject result;
  if (!node.Comment().empty()){
      result.Insert(L"Comment", winrt::Windows::Data::Json::JsonValue::CreateStringValue(node.Comment()));
    }
    winrt::Windows::Data::Json::JsonArray visualSize;
    visualSize.Append(winrt::Windows::Data::Json::JsonValue::CreateNumberValue(node.Size().x));
    visualSize.Append(winrt::Windows::Data::Json::JsonValue::CreateNumberValue(node.Size().y));
    result.Insert(L"Size", visualSize);
    winrt::Windows::Data::Json::JsonArray visualOffset;
    visualOffset.Append(winrt::Windows::Data::Json::JsonValue::CreateNumberValue(node.Offset().x));
    visualOffset.Append(winrt::Windows::Data::Json::JsonValue::CreateNumberValue(node.Offset().y));
    visualOffset.Append(winrt::Windows::Data::Json::JsonValue::CreateNumberValue(node.Offset().z));
    result.Insert(L"Opacity", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(node.Opacity()));
    auto spriteVisual = node.try_as<winrt::Windows::UI::Composition::SpriteVisual>();
    if (spriteVisual){
      result.Insert(L"Visual Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"SpriteVisual"));
      auto spriteBrush = spriteVisual.Brush();
      if (spriteBrush){
        winrt::Windows::Data::Json::JsonObject brush;
        auto colorBrush = spriteBrush.try_as<winrt::Windows::UI::Composition::CompositionColorBrush>();
        if (colorBrush){
          brush.Insert(L"Brush Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"ColorBrush"));
          auto colorString = L"rgba(" + winrt::to_hstring(colorBrush.Color().R) + L", " + winrt::to_hstring(colorBrush.Color().G) + L", " + winrt::to_hstring(colorBrush.Color().B) + L", " + winrt::to_hstring(colorBrush.Color().A) + L")";
          brush.Insert(L"Color", winrt::Windows::Data::Json::JsonValue::CreateStringValue(colorString));
          result.Insert(L"Brush", brush);
        }
      } 
    }else{
      result.Insert(L"Visual Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Visual"));
    }
    return result;
}

winrt::Windows::Data::Json::JsonObject DumpVisualTreeRecurse(winrt::Windows::UI::Composition::Visual node, winrt::hstring accessibilityId, boolean targetNodeHit) {
  winrt::Windows::Data::Json::JsonObject result;
  boolean targetNodeFound = false;
  if (targetNodeHit){
    result = PrintVisualTree(node);
  }

  auto containerNode = node.try_as<winrt::Windows::UI::Composition::ContainerVisual>();
  if (containerNode == nullptr){
    return result;
  }
  auto nodeChildren = containerNode.Children();
  winrt::Windows::Data::Json::JsonArray children;
  for (auto childVisual : nodeChildren)
  {
    if (!targetNodeHit && childVisual.Comment() == accessibilityId){
      targetNodeFound = true;
      result = DumpVisualTreeRecurse(childVisual, accessibilityId, true);
      break;
    }else if (targetNodeHit){
      children.Append(DumpVisualTreeRecurse(childVisual, accessibilityId, targetNodeHit));
    }else if (!targetNodeHit){
      auto subtree = DumpVisualTreeRecurse(childVisual, accessibilityId, targetNodeHit);
      if (subtree.Size() > 0){
        result = subtree;
        break;
      }
    }
  }
  if (targetNodeHit && children.Size() > 0) {
    result.Insert(L"Children", children);
  }
  return result;
}

winrt::Windows::Data::Json::JsonObject DumpVisualTreeHelper(winrt::Windows::Data::Json::JsonObject payloadObj) {
  auto accessibilityId = payloadObj.GetNamedString(L"accessibilityId");
  winrt::Windows::Data::Json::JsonObject visualTree;
  auto windowData = WindowData::GetFromWindow(global_hwnd);
  if (windowData == nullptr || !windowData->m_windowInited)
    return visualTree;

  auto hwndHost = windowData->m_CompositionHwndHost;
  winrt::Windows::UI::Composition::Visual root = hwndHost.RootVisual();
  visualTree = DumpVisualTreeRecurse(root, accessibilityId, false);
  return visualTree;
}

winrt::Windows::Data::Json::JsonObject DumpVisualTree(winrt::Windows::Data::Json::JsonValue payload) {
  winrt::Windows::Data::Json::JsonObject payloadObj = payload.GetObject();
  winrt::Windows::Data::Json::JsonObject result;
  result.Insert(L"Automation Tree", DumpUIATreeHelper(payloadObj));
  result.Insert(L"Visual Tree", DumpVisualTreeHelper(payloadObj));
  return result;
}

winrt::Windows::Foundation::IAsyncAction LoopServer(winrt::AutomationChannel::Server &server) {
  while (true) {
    try {
      co_await server.ProcessAllClientRequests(8603, std::chrono::milliseconds(50));
    } catch (const std::exception ex) {
    }
  }
}
