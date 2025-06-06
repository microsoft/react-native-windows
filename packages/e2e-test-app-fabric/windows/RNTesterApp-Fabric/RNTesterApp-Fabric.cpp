// RNTesterApp-Fabric.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "RNTesterApp-Fabric.h"

#include <UIAutomation.h>
#include <winrt/Windows.Data.Json.h>
#include "winrt/AutomationChannel.h"

#include "AutolinkedNativeModules.g.h"

#include "NativeModules.h"

struct CompReactPackageProvider
    : winrt::implements<CompReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    AddAttributedModules(packageBuilder, true);
  }
};

// Global Variables:
constexpr PCWSTR windowTitle = L"RNTesterApp-Fabric";
constexpr PCWSTR mainComponentName = L"RNTesterApp";

// Keep track of errors and warnings to be able to report them to automation
std::vector<std::string> g_Errors;
std::vector<std::string> g_Warnings;
HWND global_hwnd;
winrt::Microsoft::ReactNative::ReactNativeIsland *global_rootView{nullptr};
winrt::Microsoft::ReactNative::IReactContext global_reactContext{nullptr};

// Forward declarations of functions included in this code module:
winrt::Windows::Data::Json::JsonObject ListErrors(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Data::Json::JsonObject DumpVisualTree(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Foundation::IAsyncAction LoopServer(winrt::AutomationChannel::Server &server);

float ScaleFactor(HWND hwnd) noexcept {
  return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

void UpdateRootViewSizeToAppWindow(
    winrt::Microsoft::ReactNative::ReactNativeIsland const &rootView,
    winrt::Microsoft::UI::Windowing::AppWindow const &window) {
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
  auto scaleFactor = ScaleFactor(hwnd);
  winrt::Windows::Foundation::Size size{
      window.ClientSize().Width / scaleFactor, window.ClientSize().Height / scaleFactor};
  // Do not relayout when minimized
  if (window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().State() !=
      winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Minimized) {
    winrt::Microsoft::ReactNative::LayoutConstraints constraints;
    constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;
    constraints.MaximumSize = constraints.MinimumSize = size;
    rootView.Arrange(constraints, {0, 0});
  }
}

// Create and configure the ReactNativeHost
winrt::Microsoft::ReactNative::ReactNativeHost CreateReactNativeHost(
    HWND hwnd,
    const winrt::Microsoft::UI::Composition::Compositor &compositor) {
  WCHAR appDirectory[MAX_PATH];
  GetModuleFileNameW(NULL, appDirectory, MAX_PATH);
  PathCchRemoveFileSpec(appDirectory, MAX_PATH);

  auto host = winrt::Microsoft::ReactNative::ReactNativeHost();

  // Some of the images in RNTester require a user-agent header to properly fetch
  winrt::Microsoft::ReactNative::HttpSettings::SetDefaultUserAgent(
      host.InstanceSettings(), L"React Native Windows E2E Test App");

  // Include any autolinked modules
  RegisterAutolinkedNativeModulePackages(host.PackageProviders());

  host.PackageProviders().Append(winrt::make<CompReactPackageProvider>());

#if BUNDLE
  host.InstanceSettings().JavaScriptBundleFile(L"index.windows");
  host.InstanceSettings().BundleRootPath(std::wstring(L"file://").append(appDirectory).append(L"\\Bundle\\").c_str());
  host.InstanceSettings().UseFastRefresh(false);
#else
  host.InstanceSettings().JavaScriptBundleFile(L"index");
  host.InstanceSettings().UseFastRefresh(true);
#endif

#if _DEBUG
  host.InstanceSettings().UseDirectDebugger(true);
  host.InstanceSettings().UseDeveloperSupport(true);
#else
  host.InstanceSettings().UseDirectDebugger(false);
  host.InstanceSettings().UseDeveloperSupport(false);
#endif

  host.InstanceSettings().InstanceLoaded(
      [](auto /*sender*/, const winrt::Microsoft::ReactNative::InstanceLoadedEventArgs &args) {
        global_reactContext = args.Context();
      });

  // Test App hooks into JS console.log implementation to record errors/warnings
  host.InstanceSettings().NativeLogger([](winrt::Microsoft::ReactNative::LogLevel level, winrt::hstring message) {
    if (level == winrt::Microsoft::ReactNative::LogLevel::Error ||
        level == winrt::Microsoft::ReactNative::LogLevel::Fatal) {
      g_Errors.push_back(winrt::to_string(message));
    } else if (level == winrt::Microsoft::ReactNative::LogLevel::Warning) {
      g_Warnings.push_back(winrt::to_string(message));
    }
  });

  winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  winrt::Microsoft::ReactNative::Composition::CompositionUIService::SetCompositor(host.InstanceSettings(), compositor);

  return host;
}

_Use_decl_annotations_ int CALLBACK
WinMain(HINSTANCE /* instance */, HINSTANCE, PSTR /* commandLine */, int /* showCmd */) {
  // Initialize WinRT.
  winrt::init_apartment(winrt::apartment_type::single_threaded);

  // Enable per monitor DPI scaling
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  // Create a DispatcherQueue for this thread.  This is needed for Composition, Content, and
  // Input APIs.
  auto dispatcherQueueController{winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread()};

  // Create a Compositor for all Content on this thread.
  auto compositor{winrt::Microsoft::UI::Composition::Compositor()};

  // Create a top-level window.
  auto window = winrt::Microsoft::UI::Windowing::AppWindow::Create();
  window.Title(windowTitle);
  window.Resize({1000, 1000});
  window.Show();
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
  global_hwnd = hwnd;
  auto scaleFactor = ScaleFactor(hwnd);

  auto host = CreateReactNativeHost(hwnd, compositor);

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle
  host.ReloadInstance();

  // Create a RootView which will present a react-native component
  winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
  viewOptions.ComponentName(mainComponentName);
  auto rootView = winrt::Microsoft::ReactNative::ReactNativeIsland(compositor);
  rootView.ReactViewHost(winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));

  // Update the size of the RootView when the AppWindow changes size
  window.Changed([wkRootView = winrt::make_weak(rootView)](
                     winrt::Microsoft::UI::Windowing::AppWindow const &window,
                     winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const &args) {
    if (args.DidSizeChange() || args.DidVisibilityChange()) {
      if (auto rootView = wkRootView.get()) {
        UpdateRootViewSizeToAppWindow(rootView, window);
      }
    }
  });

  // Quit application when main window is closed
  window.Destroying(
      [host](winrt::Microsoft::UI::Windowing::AppWindow const & /*window*/, winrt::IInspectable const & /*args*/) {
        // Before we shutdown the application - unload the ReactNativeHost to give the javascript a chance to save any
        // state
        auto async = host.UnloadInstance();
        async.Completed([host](auto /*asyncInfo*/, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
          asyncStatus;
          assert(asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed);
          host.InstanceSettings().UIDispatcher().Post([]() { PostQuitMessage(0); });
        });
      });

  // DesktopChildSiteBridge create a ContentSite that can host the RootView ContentIsland
  auto bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(compositor, window.Id());
  bridge.Connect(rootView.Island());
  bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  rootView.ScaleFactor(scaleFactor);

  // Set the intialSize of the root view
  UpdateRootViewSizeToAppWindow(rootView, window);

  bridge.Show();

  // Set Up Servers for E2E Testing
  winrt::AutomationChannel::CommandHandler handler;
  handler.BindOperation(L"DumpVisualTree", DumpVisualTree);
  handler.BindOperation(L"ListErrors", ListErrors);
  global_rootView = &rootView;

  auto server = winrt::AutomationChannel::Server(handler);
  auto asyncAction = LoopServer(server);

  // Run the main application event loop
  dispatcherQueueController.DispatcherQueue().RunEventLoop();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  dispatcherQueueController.ShutdownQueue();

  bridge.Close();
  bridge = nullptr;

  // Destroy all Composition objects
  compositor.Close();
  compositor = nullptr;
}

void InsertStringValueIfNotEmpty(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    winrt::hstring value) {
  if (!value.empty()) {
    obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(value));
  }
}

void InsertNumberValueIfNotDefault(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    float value,
    float defaultValue = 0.0f) {
  if (value != defaultValue) {
    obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateNumberValue(value));
  }
}

void InsertIntValueIfNotDefault(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    int value,
    int defaultValue = 0) {
  if (value != defaultValue) {
    obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateNumberValue(value));
  }
}

void InsertBooleanValueIfNotDefault(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    bool value,
    bool defaultValue = false) {
  if (value != defaultValue) {
    obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateBooleanValue(value));
  }
}

void InsertLiveSettingValueIfNotDefault(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    LiveSetting value,
    LiveSetting defaultValue = LiveSetting::Off) {
  if (value != defaultValue) {
    switch (value) {
      case 0:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Off"));
        break;
      case 1:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Polite"));
        break;
      case 2:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Assertive"));
        break;
    }
  }
}

void InsertSizeValue(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    winrt::Windows::Foundation::Size value) {
  auto str = winrt::to_hstring(value.Width) + L", " + winrt::to_hstring(value.Height);
  obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(str));
}

void InsertFloat2Value(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    winrt::Windows::Foundation::Numerics::float2 value) {
  auto str = winrt::to_hstring(value.x) + L", " + winrt::to_hstring(value.y);
  obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(str));
}

void InsertFloat3Value(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    winrt::Windows::Foundation::Numerics::float3 value) {
  auto str = winrt::to_hstring(value.x) + L", " + winrt::to_hstring(value.y) + L", " + winrt::to_hstring(value.z);
  obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(str));
}

void InsertToggleStateValueIfNotDefault(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    ToggleState value,
    ToggleState defaultValue = ToggleState::ToggleState_Off) {
  if (value != defaultValue) {
    switch (value) {
      case 0:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Off"));
        break;
      case 1:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"On"));
        break;
      case 2:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Indeterminate"));
        break;
    }
  }
}

void InsertExpandCollapseStateValueIfNotDefault(
    const winrt::Windows::Data::Json::JsonObject &obj,
    winrt::hstring name,
    ExpandCollapseState value,
    ExpandCollapseState defaultValue = ExpandCollapseState::ExpandCollapseState_Collapsed) {
  if (value != defaultValue) {
    switch (value) {
      case 0:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Collapsed"));
        break;
      case 1:
        obj.Insert(name, winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Expanded"));
        break;
    }
  }
}

winrt::Windows::Data::Json::JsonObject ListErrors(winrt::Windows::Data::Json::JsonValue /*payload*/) {
  winrt::Windows::Data::Json::JsonObject result;
  winrt::Windows::Data::Json::JsonArray jsonErrors;
  winrt::Windows::Data::Json::JsonArray jsonWarnings;

  for (auto &err : g_Errors) {
    jsonErrors.InsertAt(0, winrt::Windows::Data::Json::JsonValue::CreateStringValue(winrt::to_hstring(err)));
  }
  g_Errors.clear();
  for (auto &warn : g_Warnings) {
    jsonWarnings.InsertAt(0, winrt::Windows::Data::Json::JsonValue::CreateStringValue(winrt::to_hstring(warn)));
  }
  g_Warnings.clear();

  result.Insert(L"errors", jsonErrors);
  result.Insert(L"warnings", jsonWarnings);

  return result;
}

void DumpUIAPatternInfo(IUIAutomationElement *pTarget, const winrt::Windows::Data::Json::JsonObject &result) {
  BSTR value = nullptr;
  BOOL isReadOnly;
  double now;
  double min;
  double max;
  ToggleState toggleState;
  ExpandCollapseState expandCollapseState;
  HRESULT hr;
  BOOL isSelected;
  BOOL multipleSelection;
  BOOL selectionRequired;
  BSTR text = nullptr;
  BOOL horizontallyScrollable;
  BSTR annotationAuthor = nullptr;
  BSTR annotationTypeName = nullptr;
  BSTR annotationDateTime = nullptr;
  int annotationTypeID = 0;

  // Dump IValueProvider Information
  IValueProvider *valuePattern;
  hr = pTarget->GetCurrentPattern(UIA_ValuePatternId, reinterpret_cast<IUnknown **>(&valuePattern));
  if (SUCCEEDED(hr) && valuePattern) {
    hr = valuePattern->get_Value(&value);
    if (SUCCEEDED(hr)) {
      InsertStringValueIfNotEmpty(result, L"ValuePattern.Value", value);
    }
    hr = valuePattern->get_IsReadOnly(&isReadOnly);
    if (SUCCEEDED(hr)) {
      InsertBooleanValueIfNotDefault(result, L"ValuePattern.IsReadOnly", isReadOnly, false);
    }
    valuePattern->Release();
  }

  // Dump IRangeValueProvider Information
  IRangeValueProvider *rangeValuePattern;
  hr = pTarget->GetCurrentPattern(UIA_RangeValuePatternId, reinterpret_cast<IUnknown **>(&rangeValuePattern));
  if (SUCCEEDED(hr) && rangeValuePattern) {
    hr = rangeValuePattern->get_Value(&now);
    if (SUCCEEDED(hr)) {
      result.Insert(L"RangeValuePattern.Value", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(now));
    }
    hr = rangeValuePattern->get_Minimum(&min);
    if (SUCCEEDED(hr)) {
      result.Insert(L"RangeValuePattern.Minimum", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(min));
    }
    hr = rangeValuePattern->get_Maximum(&max);
    if (SUCCEEDED(hr)) {
      result.Insert(L"RangeValuePattern.Maximum", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(max));
    }
    hr = rangeValuePattern->get_IsReadOnly(&isReadOnly);
    if (SUCCEEDED(hr)) {
      InsertBooleanValueIfNotDefault(result, L"RangeValuePattern.IsReadOnly", isReadOnly, false);
    }
    rangeValuePattern->Release();
  }

  // Dump IToggleProvider Information
  IToggleProvider *togglePattern;
  hr = pTarget->GetCurrentPattern(UIA_TogglePatternId, reinterpret_cast<IUnknown **>(&togglePattern));
  if (SUCCEEDED(hr) && togglePattern) {
    hr = togglePattern->get_ToggleState(&toggleState);
    if (SUCCEEDED(hr)) {
      InsertToggleStateValueIfNotDefault(result, L"TogglePattern.ToggleState", toggleState);
    }
    togglePattern->Release();
  }

  // Dump IExpandCollapseProvider Information
  IExpandCollapseProvider *expandCollapsePattern;
  hr = pTarget->GetCurrentPattern(UIA_ExpandCollapsePatternId, reinterpret_cast<IUnknown **>(&expandCollapsePattern));
  if (SUCCEEDED(hr) && expandCollapsePattern) {
    hr = expandCollapsePattern->get_ExpandCollapseState(&expandCollapseState);
    if (SUCCEEDED(hr)) {
      InsertExpandCollapseStateValueIfNotDefault(
          result, L"ExpandCollapsePattern.ExpandCollapseState", expandCollapseState);
    }
    expandCollapsePattern->Release();
  }

  // Dump ISelectionItemProvider Information
  ISelectionItemProvider *selectionItemPattern;
  hr = pTarget->GetCurrentPattern(UIA_SelectionItemPatternId, reinterpret_cast<IUnknown **>(&selectionItemPattern));
  if (SUCCEEDED(hr) && selectionItemPattern) {
    hr = selectionItemPattern->get_IsSelected(&isSelected);
    if (SUCCEEDED(hr)) {
      InsertBooleanValueIfNotDefault(result, L"SelectionItemPattern.IsSelected", isSelected);
    }
    selectionItemPattern->Release();
  }

  // Dump ISelectionProvider Information
  ISelectionProvider *selectionPattern;
  hr = pTarget->GetCurrentPattern(UIA_SelectionPatternId, reinterpret_cast<IUnknown **>(&selectionPattern));
  if (SUCCEEDED(hr) && selectionPattern) {
    hr = selectionPattern->get_CanSelectMultiple(&multipleSelection);
    if (SUCCEEDED(hr)) {
      InsertBooleanValueIfNotDefault(result, L"SelectionPattern.CanSelectMultiple", multipleSelection, false);
    }
    hr = selectionPattern->get_IsSelectionRequired(&selectionRequired);
    if (SUCCEEDED(hr)) {
      InsertBooleanValueIfNotDefault(result, L"SelectionPattern.IsSelectionRequired", selectionRequired, false);
    }
    selectionPattern->Release();
  }

  // Dump ITextRangeProvider Information
  winrt::com_ptr<ITextProvider> textPattern;
  hr = pTarget->GetCurrentPattern(UIA_TextPatternId, reinterpret_cast<IUnknown **>(textPattern.put()));
  if (SUCCEEDED(hr) && textPattern) {
    winrt::com_ptr<ITextRangeProvider> textRangePattern;
    hr = textPattern->get_DocumentRange(textRangePattern.put());
    if (SUCCEEDED(hr) && textRangePattern) {
      textRangePattern->GetText(20, &text);
      if (SUCCEEDED(hr)) {
        InsertStringValueIfNotEmpty(result, L"TextRangePattern.GetText", text);
      }
    }
  }

  // Dump IAnnotationProvider Information
  winrt::com_ptr<IAnnotationProvider> annotationProvider;
  hr = pTarget->GetCurrentPattern(UIA_AnnotationPatternId, reinterpret_cast<IUnknown **>(annotationProvider.put()));
  if (SUCCEEDED(hr) && annotationProvider) {
    hr = annotationProvider->get_AnnotationTypeId(&annotationTypeID);
    if (SUCCEEDED(hr)) {
      InsertIntValueIfNotDefault(result, L"AnnotationPattern.TypeId", annotationTypeID, 0);
    }
    hr = annotationProvider->get_AnnotationTypeName(&annotationTypeName);
    if (SUCCEEDED(hr)) {
      InsertStringValueIfNotEmpty(result, L"AnnotationPattern.TypeName", annotationTypeName);
    }
    hr = annotationProvider->get_Author(&annotationAuthor);
    if (SUCCEEDED(hr)) {
      InsertStringValueIfNotEmpty(result, L"AnnotationPattern.Author", annotationAuthor);
    }
    hr = annotationProvider->get_DateTime(&annotationDateTime);
    if (SUCCEEDED(hr)) {
      InsertStringValueIfNotEmpty(result, L"AnnotationPattern.DateTime", annotationDateTime);
    }
  }

  // Dump IScrollProvider Information
  winrt::com_ptr<IScrollProvider> scrollPattern;
  hr = pTarget->GetCurrentPattern(UIA_ScrollPatternId, reinterpret_cast<IUnknown **>(scrollPattern.put()));
  if (SUCCEEDED(hr) && scrollPattern) {
    hr = scrollPattern->get_HorizontallyScrollable(&horizontallyScrollable);
    if (SUCCEEDED(hr)) {
      InsertBooleanValueIfNotDefault(result, L"ScrollPattern.HorizontallyScrollable", horizontallyScrollable, false);
    }
  }

  ::SysFreeString(text);
  ::SysFreeString(value);
}

winrt::Windows::Data::Json::JsonObject DumpUIATreeRecurse(
    IUIAutomationElement *pTarget,
    IUIAutomationTreeWalker *pWalker) {
  winrt::Windows::Data::Json::JsonObject result;
  BSTR automationId;
  CONTROLTYPEID controlType;
  BSTR helpText;
  BOOL isEnabled;
  BOOL isKeyboardFocusable;
  BSTR localizedControlType;
  BSTR name;
  int positionInSet = 0;
  int sizeOfSet = 0;
  int level = 0;
  LiveSetting liveSetting = LiveSetting::Off;
  BSTR itemStatus;
  BSTR itemType;
  BSTR accessKey;

  pTarget->get_CurrentAutomationId(&automationId);
  pTarget->get_CurrentControlType(&controlType);
  pTarget->get_CurrentHelpText(&helpText);
  pTarget->get_CurrentIsEnabled(&isEnabled);
  pTarget->get_CurrentIsKeyboardFocusable(&isKeyboardFocusable);
  pTarget->get_CurrentLocalizedControlType(&localizedControlType);
  pTarget->get_CurrentName(&name);
  pTarget->get_CurrentItemStatus(&itemStatus);
  pTarget->get_CurrentItemType(&itemType);
  pTarget->get_CurrentAccessKey(&accessKey);
  IUIAutomationElement4 *pTarget4;
  HRESULT hr = pTarget->QueryInterface(__uuidof(IUIAutomationElement4), reinterpret_cast<void **>(&pTarget4));
  if (SUCCEEDED(hr) && pTarget4) {
    pTarget4->get_CurrentPositionInSet(&positionInSet);
    pTarget4->get_CurrentSizeOfSet(&sizeOfSet);
    pTarget4->get_CurrentLiveSetting(&liveSetting);
    pTarget4->get_CurrentLevel(&level);
    pTarget4->Release();
  }
  result.Insert(L"AutomationId", winrt::Windows::Data::Json::JsonValue::CreateStringValue(automationId));
  result.Insert(L"ControlType", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(controlType));
  InsertStringValueIfNotEmpty(result, L"HelpText", helpText);
  InsertBooleanValueIfNotDefault(result, L"IsEnabled", isEnabled, true);
  InsertBooleanValueIfNotDefault(result, L"IsKeyboardFocusable", isKeyboardFocusable);
  result.Insert(
      L"LocalizedControlType", winrt::Windows::Data::Json::JsonValue::CreateStringValue(localizedControlType));
  InsertStringValueIfNotEmpty(result, L"Name", name);
  InsertIntValueIfNotDefault(result, L"PositionInSet", positionInSet);
  InsertIntValueIfNotDefault(result, L"SizeofSet", sizeOfSet);
  InsertIntValueIfNotDefault(result, L"Level", level);
  InsertLiveSettingValueIfNotDefault(result, L"LiveSetting", liveSetting);
  InsertStringValueIfNotEmpty(result, L"ItemStatus", itemStatus);
  InsertStringValueIfNotEmpty(result, L"ItemType", itemType);
  InsertStringValueIfNotEmpty(result, L"AccessKey", accessKey);
  DumpUIAPatternInfo(pTarget, result);

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
    result.Insert(L"__Children", children);
  }
  ::SysFreeString(automationId);
  ::SysFreeString(helpText);
  ::SysFreeString(localizedControlType);
  ::SysFreeString(name);
  ::SysFreeString(itemStatus);
  return result;
}

winrt::Windows::Data::Json::JsonObject DumpUIATreeHelper(winrt::Windows::Data::Json::JsonObject payloadObj) {
  auto accessibilityId = payloadObj.GetNamedString(L"accessibilityId");

  winrt::Windows::Data::Json::JsonObject result;

  IUIAutomation *pAutomation;
  IUIAutomationElement *pRootElement;
  IUIAutomationTreeWalker *pWalker;

  winrt::check_hresult(
      CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAutomation)));
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

winrt::Windows::Data::Json::JsonObject PrintVisualTree(winrt::Microsoft::UI::Composition::Visual node) {
  winrt::Windows::Data::Json::JsonObject result;
  InsertStringValueIfNotEmpty(result, L"Comment", node.Comment());
  InsertFloat2Value(result, L"Size", node.Size());
  InsertFloat3Value(result, L"Offset", node.Offset());
  InsertNumberValueIfNotDefault(result, L"Opacity", node.Opacity(), 1.0f);
  auto spriteVisual = node.try_as<winrt::Microsoft::UI::Composition::SpriteVisual>();
  if (spriteVisual) {
    result.Insert(L"Visual Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"SpriteVisual"));
    auto spriteBrush = spriteVisual.Brush();
    if (spriteBrush) {
      winrt::Windows::Data::Json::JsonObject brush;
      auto colorBrush = spriteBrush.try_as<winrt::Microsoft::UI::Composition::CompositionColorBrush>();
      if (colorBrush) {
        brush.Insert(L"Brush Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"ColorBrush"));
        auto colorString = L"rgba(" + winrt::to_hstring(colorBrush.Color().R) + L", " +
            winrt::to_hstring(colorBrush.Color().G) + L", " + winrt::to_hstring(colorBrush.Color().B) + L", " +
            winrt::to_hstring(colorBrush.Color().A) + L")";
        brush.Insert(L"Color", winrt::Windows::Data::Json::JsonValue::CreateStringValue(colorString));
        result.Insert(L"Brush", brush);
      }
    }
  } else {
    result.Insert(L"Visual Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Visual"));
  }
  return result;
}

winrt::Windows::Data::Json::JsonObject DumpVisualTreeRecurse(
    winrt::Microsoft::UI::Composition::Visual node,
    winrt::hstring accessibilityId,
    boolean targetNodeHit) {
  winrt::Windows::Data::Json::JsonObject result;
  if (targetNodeHit) {
    result = PrintVisualTree(node);
  }

  auto containerNode = node.try_as<winrt::Microsoft::UI::Composition::ContainerVisual>();
  if (containerNode == nullptr) {
    return result;
  }
  auto nodeChildren = containerNode.Children();
  winrt::Windows::Data::Json::JsonArray children;
  for (auto childVisual : nodeChildren) {
    if (!targetNodeHit && childVisual.Comment() == accessibilityId) {
      result = DumpVisualTreeRecurse(childVisual, accessibilityId, true);
      break;
    } else if (targetNodeHit) {
      children.Append(DumpVisualTreeRecurse(childVisual, accessibilityId, targetNodeHit));
    } else if (!targetNodeHit) {
      auto subtree = DumpVisualTreeRecurse(childVisual, accessibilityId, targetNodeHit);
      if (subtree.Size() > 0) {
        result = subtree;
        break;
      }
    }
  }
  if (targetNodeHit && children.Size() > 0) {
    result.Insert(L"__Children", children);
  }
  return result;
}

winrt::Windows::Data::Json::JsonObject PrintNativeComponentTree(winrt::Microsoft::ReactNative::ComponentView node) {
  winrt::Windows::Data::Json::JsonObject result;

  result.Insert(L"Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(winrt::get_class_name(node)));

  if (auto viewComponent = node.try_as<winrt::Microsoft::ReactNative::Composition::ViewComponentView>()) {
    winrt::Windows::Data::Json::JsonObject props;

    auto viewProps = viewComponent.ViewProps();
    InsertNumberValueIfNotDefault(props, L"Opacity", viewProps.Opacity(), 1.0f);
    InsertStringValueIfNotEmpty(props, L"TestId", viewProps.TestId());
    InsertStringValueIfNotEmpty(props, L"AccessibilityLabel", viewProps.AccessibilityLabel());
    if (auto imageComponent = node.try_as<winrt::Microsoft::ReactNative::Composition::ImageComponentView>()) {
      winrt::Windows::Data::Json::JsonArray sources;
      auto imageProps = viewProps.as<winrt::Microsoft::ReactNative::ImageProps>();
      if (imageProps.Sources().Size() != 0) {
        for (auto imageSource : imageProps.Sources()) {
          winrt::Windows::Data::Json::JsonObject source;
          InsertStringValueIfNotEmpty(source, L"Uri", imageSource.Uri());
          if (imageSource.Size().Width || imageSource.Size().Height) {
            InsertSizeValue(source, L"Size", imageSource.Size());
          }
          InsertNumberValueIfNotDefault(source, L"Scale", imageSource.Scale(), 1.0f);
          InsertStringValueIfNotEmpty(source, L"Bundle", imageSource.Bundle());
          switch (imageSource.Type()) {
            case winrt::Microsoft::ReactNative::ImageSourceType::Invalid:
              source.Insert(L"Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Invalid"));
              break;
            case winrt::Microsoft::ReactNative::ImageSourceType::Local:
              source.Insert(L"Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Local"));
              break;
            case winrt::Microsoft::ReactNative::ImageSourceType::Remote:
              source.Insert(L"Type", winrt::Windows::Data::Json::JsonValue::CreateStringValue(L"Remote"));
              break;
          }
          sources.Append(source);
        }
        props.Insert(L"Sources", sources);
      }
    }
    result.Insert(L"_Props", props);
  }
  return result;
}

winrt::Windows::Data::Json::JsonObject DumpNativeComponentTreeRecurse(
    winrt::Microsoft::ReactNative::ComponentView node,
    winrt::hstring accessibilityId,
    boolean targetNodeHit) {
  winrt::Windows::Data::Json::JsonObject result;
  if (targetNodeHit) {
    result = PrintNativeComponentTree(node);
  }

  auto nodeChildren = node.Children();
  winrt::Windows::Data::Json::JsonArray children;
  for (auto childNode : nodeChildren) {
    auto childNodeAsViewComponent = childNode.try_as<winrt::Microsoft::ReactNative::Composition::ViewComponentView>();
    if (!targetNodeHit && childNodeAsViewComponent &&
        childNodeAsViewComponent.ViewProps().TestId() == accessibilityId) {
      result = DumpNativeComponentTreeRecurse(childNode, accessibilityId, true);
      break;
    } else if (targetNodeHit) {
      children.Append(DumpNativeComponentTreeRecurse(childNode, accessibilityId, targetNodeHit));
    } else if (!targetNodeHit) {
      auto subtree = DumpNativeComponentTreeRecurse(childNode, accessibilityId, targetNodeHit);
      if (subtree.Size() > 0) {
        result = subtree;
        break;
      }
    }
  }
  if (targetNodeHit && children.Size() > 0) {
    result.Insert(L"__Children", children);
  }
  return result;
}

winrt::Windows::Data::Json::JsonObject DumpVisualTreeHelper(winrt::Windows::Data::Json::JsonObject payloadObj) {
  auto accessibilityId = payloadObj.GetNamedString(L"accessibilityId");
  winrt::Windows::Data::Json::JsonObject visualTree;
  auto root = global_rootView->RootVisual();
  visualTree = DumpVisualTreeRecurse(root, accessibilityId, false);
  return visualTree;
}

winrt::Windows::Data::Json::JsonObject DumpNativeComponentTreeHelper(
    winrt::Windows::Data::Json::JsonObject payloadObj) {
  auto accessibilityId = payloadObj.GetNamedString(L"accessibilityId");
  winrt::Windows::Data::Json::JsonObject visualTree;
  auto rootTag = global_rootView->RootTag();
  if (auto root = winrt::Microsoft::ReactNative::Composition::CompositionUIService::ComponentFromReactTag(
          global_reactContext, rootTag)) {
    visualTree = DumpNativeComponentTreeRecurse(root, accessibilityId, false);
  }
  return visualTree;
}

winrt::Windows::Data::Json::JsonObject DumpVisualTree(winrt::Windows::Data::Json::JsonValue payload) {
  winrt::Windows::Data::Json::JsonObject payloadObj = payload.GetObject();
  winrt::Windows::Data::Json::JsonObject result;
  result.Insert(L"Automation Tree", DumpUIATreeHelper(payloadObj));
  result.Insert(L"Visual Tree", DumpVisualTreeHelper(payloadObj));
  result.Insert(L"Component Tree", DumpNativeComponentTreeHelper(payloadObj));
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
