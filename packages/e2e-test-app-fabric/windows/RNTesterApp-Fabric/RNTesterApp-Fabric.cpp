// RNTesterApp-Fabric.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "RNTesterApp-Fabric.h"

#include <UIAutomation.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Graphics.Capture.h>
#include "winrt/AutomationChannel.h"

#include <windows.graphics.directx.direct3d11.interop.h>
#include "d3d11.h"

// Includes from sample-custom-component
#include <winrt/SampleCustomComponent.h>

#include <Shlwapi.h>
#include <wil/resource.h>
#include <wincodec.h>

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
winrt::Microsoft::ReactNative::ReactNativeIsland global_rootView{nullptr};
winrt::Microsoft::ReactNative::IReactContext global_reactContext{nullptr};

// Forward declarations of functions included in this code module:
winrt::Windows::Data::Json::JsonObject ListErrors(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Data::Json::JsonObject DumpVisualTree(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::IJsonValue> CreateScreenshot(
    winrt::Windows::Data::Json::JsonValue /*payload*/);
winrt::Windows::Foundation::IAsyncAction LoopServer(winrt::AutomationChannel::Server &server);

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
  host.PackageProviders().Append(winrt::SampleCustomComponent::ReactPackageProvider());

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
  auto reactNativeWindow = winrt::Microsoft::ReactNative::ReactNativeWindow::CreateFromCompositor(compositor);
  auto appWindow = reactNativeWindow.AppWindow();
  appWindow.Title(windowTitle);
  appWindow.Resize({1000, 1000});
  appWindow.Show();
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(appWindow.Id());
  global_hwnd = hwnd;

  CreateScreenshot(winrt::Windows::Data::Json::JsonValue::CreateNullValue()); // Temp testing

  auto host = CreateReactNativeHost(hwnd, compositor);

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle
  host.ReloadInstance();

  // Create a RootView which will present a react-native component
  winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
  viewOptions.ComponentName(mainComponentName);
  reactNativeWindow.ReactNativeIsland().ReactViewHost(
      winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));

  // Quit application when main window is closed
  appWindow.Destroying(
      [host](winrt::Microsoft::UI::Windowing::AppWindow const & /*appWindow*/, winrt::IInspectable const & /*args*/) {
        // Before we shutdown the application - unload the ReactNativeHost to give the javascript a chance to save any
        // state
        auto async = host.UnloadInstance();
        async.Completed([host](auto /*asyncInfo*/, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
          asyncStatus;
          assert(asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed);
          host.InstanceSettings().UIDispatcher().Post([]() { PostQuitMessage(0); });
        });
      });

  // Set Up Servers for E2E Testing
  winrt::AutomationChannel::CommandHandler handler;
  // handler.BindOperation(L"CreateScreenshot", CreateScreenshot);
  handler.BindAsyncOperation(L"CreateScreenshot", CreateScreenshot);
  handler.BindOperation(L"DumpVisualTree", DumpVisualTree);
  handler.BindOperation(L"ListErrors", ListErrors);
  global_rootView = reactNativeWindow.ReactNativeIsland();

  auto server = winrt::AutomationChannel::Server(handler);
  auto asyncAction = LoopServer(server);

  // Run the main application event loop
  dispatcherQueueController.DispatcherQueue().RunEventLoop();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  dispatcherQueueController.ShutdownQueue();

  global_rootView = nullptr;

  reactNativeWindow.Close();
  reactNativeWindow = nullptr;

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
  int headingLevel = 0;
  LiveSetting liveSetting = LiveSetting::Off;
  BSTR itemStatus;
  BSTR itemType;
  BSTR accessKey;
  BSTR description = nullptr;

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
  IUIAutomationElement6 *pTarget6;
  hr = pTarget->QueryInterface(__uuidof(IUIAutomationElement6), reinterpret_cast<void **>(&pTarget6));
  if (SUCCEEDED(hr) && pTarget6) {
    pTarget6->get_CurrentFullDescription(&description);
  }
  IUIAutomationElement8 *pTarget8;
  hr = pTarget->QueryInterface(__uuidof(IUIAutomationElement8), reinterpret_cast<void **>(&pTarget8));
  if (SUCCEEDED(hr) && pTarget8) {
    pTarget8->get_CurrentHeadingLevel(&headingLevel);
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
  InsertIntValueIfNotDefault(result, L"HeadingLevel", headingLevel, HeadingLevel_None);
  InsertIntValueIfNotDefault(result, L"SizeofSet", sizeOfSet);
  InsertIntValueIfNotDefault(result, L"Level", level);
  InsertLiveSettingValueIfNotDefault(result, L"LiveSetting", liveSetting);
  InsertStringValueIfNotEmpty(result, L"ItemStatus", itemStatus);
  InsertStringValueIfNotEmpty(result, L"ItemType", itemType);
  InsertStringValueIfNotEmpty(result, L"AccessKey", accessKey);
  InsertStringValueIfNotEmpty(result, L"Description", description);
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
  ::SysFreeString(itemType);
  ::SysFreeString(accessKey);
  ::SysFreeString(description);
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
  auto root = global_rootView.RootVisual();
  visualTree = DumpVisualTreeRecurse(root, accessibilityId, false);
  return visualTree;
}

winrt::Windows::Data::Json::JsonObject DumpNativeComponentTreeHelper(
    winrt::Windows::Data::Json::JsonObject payloadObj) {
  auto accessibilityId = payloadObj.GetNamedString(L"accessibilityId");
  winrt::Windows::Data::Json::JsonObject visualTree;
  auto rootTag = global_rootView.RootTag();
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

winrt::com_ptr<ID3D11Device> CreateD3DDevice() noexcept {
  // This flag adds support for surfaces with a different color channel ordering than the API default.
  // You need it for compatibility with Direct2D.
  UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  // #if defined(_DEBUG)
  //   creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
  // #endif

  // This array defines the set of DirectX hardware feature levels this app  supports.
  // The ordering is important and you should  preserve it.
  // Don't forget to declare your app's minimum required feature level in its
  // description.  All apps are assumed to support 9.1 unless otherwise stated.
  D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1};

  winrt::com_ptr<ID3D11Device> d3dDevice;

  D3D11CreateDevice(
      nullptr, // specify null to use the default adapter
      D3D_DRIVER_TYPE_HARDWARE,
      0,
      creationFlags, // optionally set debug and Direct2D compatibility flags
      featureLevels, // list of feature levels this app can support
      ARRAYSIZE(featureLevels), // number of possible feature levels
      D3D11_SDK_VERSION,
      d3dDevice.put(), // returns the Direct3D device created
      nullptr /*&m_featureLevel*/, // returns feature level of device created
      nullptr /*&context*/ // returns the device immediate context
  );

  return d3dDevice;
}

winrt::com_ptr<ID3D11Texture2D> CopyD3DTexture(
    winrt::com_ptr<ID3D11Device> &device,
    winrt::com_ptr<ID3D11Texture2D> const &texture,
    bool asStagingTexture) {
  winrt::com_ptr<ID3D11DeviceContext> context;
  device->GetImmediateContext(context.put());

  D3D11_TEXTURE2D_DESC desc = {};
  texture->GetDesc(&desc);
  // Clear flags that we don't need
  desc.Usage = asStagingTexture ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;
  desc.BindFlags = asStagingTexture ? 0 : D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = asStagingTexture ? D3D11_CPU_ACCESS_READ : 0;
  desc.MiscFlags = 0;

  // Create and fill the texture copy
  winrt::com_ptr<ID3D11Texture2D> textureCopy;
  winrt::check_hresult(device->CreateTexture2D(&desc, nullptr, textureCopy.put()));
  context->CopyResource(textureCopy.get(), texture.get());

  return textureCopy;
}

void WicSaveTexture(
    LPCWSTR fileName,
    ID3D11DeviceContext *context,
    ID3D11Texture2D *texture,
    UINT subresourceIndex,
    bool ignoreAlpha) {
  D3D11_TEXTURE2D_DESC desc;
  texture->GetDesc(&desc);

  winrt::com_ptr<ID3D11Texture2D> cpuTexture;
  if (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ) {
    cpuTexture.copy_from(texture);
  } else {
    winrt::com_ptr<ID3D11Device> device;
    context->GetDevice(device.put());

    D3D11_TEXTURE2D_DESC cpuDesc = {
        .Width = desc.Width,
        .Height = desc.Height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = desc.Format,
        .SampleDesc = {1, 0},
        .Usage = D3D11_USAGE_STAGING,
        .CPUAccessFlags = D3D11_CPU_ACCESS_READ,
    };

    winrt::check_hresult(device->CreateTexture2D(&cpuDesc, nullptr, cpuTexture.put()));
    context->CopySubresourceRegion(
        (ID3D11Resource *)cpuTexture.get(), 0, 0, 0, 0, (ID3D11Resource *)texture, subresourceIndex, nullptr);
    subresourceIndex = 0;
  }

  D3D11_MAPPED_SUBRESOURCE mapped;
  winrt::check_hresult(context->Map((ID3D11Resource *)cpuTexture.get(), subresourceIndex, D3D11_MAP_READ, 0, &mapped));

  winrt::com_ptr<IWICStream> stream;

  winrt::com_ptr<IWICImagingFactory> wicFactory;
  winrt::check_hresult(CoCreateInstance(
      CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, wicFactory.put_void()));

  winrt::check_hresult(wicFactory->CreateStream(stream.put()));
  winrt::check_hresult(stream->InitializeFromFilename(fileName, GENERIC_WRITE));

  /*
  LPWSTR fileExtension = PathFindExtensionW(fileName);

  bool isBmp = CompareStringOrdinal(fileExtension, -1, L".bmp", 4, true) == 0;
  bool isPng = CompareStringOrdinal(fileExtension, -1, L".png", 4, true) == 0;
  bool isJpeg = CompareStringOrdinal(fileExtension, -1, L".jpg", 4, true) == 0 || CompareStringOrdinal(fileExtension,
  -1, L".jpeg", 5, true) == 0; bool isHeif = CompareStringOrdinal(fileExtension, -1, L".heif", 5, true) == 0 ||
  CompareStringOrdinal(fileExtension, -1, L".heic", 5, true) == 0; assert(isBmp || isPng || isJpeg || isHeif);
  */
  bool isBmp = false, isJpeg = false;
  bool isPng = true;

  GUID container = isBmp ? GUID_ContainerFormatBmp
      : isPng            ? GUID_ContainerFormatPng
      : isJpeg           ? GUID_ContainerFormatJpeg
                         : GUID_ContainerFormatHeif;

  winrt::com_ptr<IWICBitmapEncoder> encoder;
  winrt::check_hresult(wicFactory->CreateEncoder(container, NULL, encoder.put()));
  winrt::check_hresult(encoder->Initialize(stream.get(), WICBitmapEncoderNoCache));

  winrt::com_ptr<IWICBitmapFrameEncode> frame;
  winrt::com_ptr<IPropertyBag2> options;
  winrt::check_hresult(encoder->CreateNewFrame(frame.put(), options.put()));

  WICPixelFormatGUID nativeFormat;
  if (desc.Format == DXGI_FORMAT_R8_UNORM) {
    nativeFormat = GUID_WICPixelFormat8bppGray;
  } else if (desc.Format == DXGI_FORMAT_B8G8R8X8_UNORM) {
    nativeFormat = GUID_WICPixelFormat32bppBGR;
  } else if (desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM) {
    nativeFormat = ignoreAlpha ? GUID_WICPixelFormat32bppRGB : GUID_WICPixelFormat32bppRGBA;
  } else if (desc.Format == DXGI_FORMAT_B8G8R8A8_UNORM) {
    nativeFormat = ignoreAlpha ? GUID_WICPixelFormat32bppBGR : GUID_WICPixelFormat32bppBGRA;
  } else {
    assert(!"unsupported texture format");
    nativeFormat = GUID_NULL;
  }

  if (isBmp && (desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM || desc.Format == DXGI_FORMAT_B8G8R8A8_UNORM)) {
    PROPBAG2 key;
    key.pstrName = const_cast<wchar_t *>(L"EnableV5Header32bppBGRA");
    VARIANT value;
    value.vt = VT_BOOL;
    value.boolVal = VARIANT_TRUE;

    winrt::check_hresult(options->Write(1, &key, &value));
  }
  /*
    else if (isPng)
    {
        // only use "up" filter for faster performance, with minor compression hit
        PROPBAG2 key = { .pstrName = L"FilterOption" };
        VARIANT value =
        {
            .vt = VT_UI1,
            .bVal = WICPngFilterUp,
        };
        winrt::check_hresult(IPropertyBag2_Write(options, 1, &key, &value));
    }
    */

  winrt::check_hresult(frame->Initialize(options.get()));
  winrt::check_hresult(frame->SetSize(desc.Width, desc.Height));

  if (isBmp && desc.Format == DXGI_FORMAT_R8_UNORM) {
    nativeFormat = GUID_WICPixelFormat8bppIndexed;

    winrt::com_ptr<IWICPalette> palette;
    winrt::check_hresult(wicFactory->CreatePalette(palette.put()));
    winrt::check_hresult(palette->InitializePredefined(WICBitmapPaletteTypeFixedGray256, FALSE));
    winrt::check_hresult(frame->SetPalette(palette.get()));
  }

  GUID convertFormat = nativeFormat;
  winrt::check_hresult(frame->SetPixelFormat(&convertFormat));

  winrt::com_ptr<IWICBitmapSource> source;

  if (!IsEqualGUID(nativeFormat, convertFormat)) {
    if (!source) {
      winrt::com_ptr<IWICBitmap> bitmap;
      winrt::check_hresult(wicFactory->CreateBitmapFromMemory(
          desc.Width,
          desc.Height,
          nativeFormat,
          mapped.RowPitch,
          mapped.RowPitch * desc.Height,
          (BYTE *)mapped.pData,
          bitmap.put()));
      bitmap.as(source);
    }

    winrt::com_ptr<IWICFormatConverter> converter;
    winrt::check_hresult(wicFactory->CreateFormatConverter(converter.put()));
    winrt::check_hresult(converter->Initialize(
        source.get(), convertFormat, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom));
    converter.as(source);
  }

  if (source) {
    winrt::check_hresult(frame->WriteSource(source.get(), nullptr));
  } else {
    winrt::check_hresult(
        frame->WritePixels(desc.Height, mapped.RowPitch, mapped.RowPitch * desc.Height, (BYTE *)mapped.pData));
  }

  context->Unmap((ID3D11Resource *)cpuTexture.get(), subresourceIndex);

  winrt::check_hresult(frame->Commit());
  winrt::check_hresult(encoder->Commit());
  winrt::check_hresult(stream->Commit(STGC_DEFAULT));
}

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::IInspectable> TakeScreenshotAsync() {
  auto d3dDevice = CreateD3DDevice();

  auto windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(global_hwnd);
  co_await winrt::Windows::Graphics::Capture::GraphicsCaptureAccess::RequestAccessAsync(
      winrt::Windows::Graphics::Capture::GraphicsCaptureAccessKind::Programmatic);

  auto item = winrt::Windows::Graphics::Capture::GraphicsCaptureItem::TryCreateFromWindowId(
      winrt::Windows::UI::WindowId{windowId.Value});

  // Grab the apartment context so we can return to it.
  winrt::apartment_context context;

  winrt::com_ptr<ID3D11DeviceContext> d3dContext;
  d3dDevice->GetImmediateContext(d3dContext.put());

  winrt::com_ptr<IDXGIDevice> dxgIDevice;
  d3dDevice.as(dxgIDevice);

  winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device;
  winrt::com_ptr<IInspectable> inspectableSurface;
  winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgIDevice.get(), inspectableSurface.put()));

  inspectableSurface.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(device);

  // Creating our frame pool with CreateFreeThreaded means that we
  // will be called back from the frame pool's internal worker thread
  // instead of the thread we are currently on. It also disables the
  // DispatcherQueue requirement.
  auto framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
      device, winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 1, item.Size());
  auto session = framePool.CreateCaptureSession(item);

  wil::shared_event captureEvent(wil::EventOptions::ManualReset);
  winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame{nullptr};
  framePool.FrameArrived([&frame, captureEvent](auto &framePool, auto &) {
    frame = framePool.TryGetNextFrame();

    // Complete the operation
    captureEvent.SetEvent();
  });

  session.StartCapture();
  co_await winrt::resume_on_signal(captureEvent.get());
  co_await context;

  // End the capture
  session.Close();
  framePool.Close();

  winrt::com_ptr<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess> dxgiInterfaceAccess;
  frame.Surface().as(dxgiInterfaceAccess);
  winrt::com_ptr<ID3D11Texture2D> texture;
  winrt::check_hresult(dxgiInterfaceAccess->GetInterface(IID_ID3D11Texture2D, texture.put_void()));
  auto result = CopyD3DTexture(d3dDevice, texture, true);

  WicSaveTexture(
      L"D:\\UserRepos\\react-native-windows2\\packages\\e2e-test-app-fabric\\test.png",
      d3dContext.get(),
      result.get(),
      0,
      false);

  co_return {};
}

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::IJsonValue> CreateScreenshot(
    winrt::Windows::Data::Json::JsonValue /*payload*/) {
  winrt::Windows::Data::Json::JsonObject visualTree;
  auto root = global_rootView.RootVisual();

  co_await TakeScreenshotAsync();

  // visualTree = DumpVisualTreeRecurse(root, accessibilityId);

  co_return visualTree;
}

winrt::Windows::Foundation::IAsyncAction LoopServer(winrt::AutomationChannel::Server &server) {
  while (true) {
    try {
      co_await server.ProcessAllClientRequests(8603, std::chrono::milliseconds(50));
    } catch (const std::exception ex) {
    }
  }
}
