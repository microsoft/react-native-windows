// RNTesterApp-Fabric.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "RNTesterApp-Fabric.h"

#include "../../../../vnext/codegen/NativeDeviceInfoSpec.g.h"
#include "winrt/AutomationChannel.h"

#include <DispatcherQueue.h>
#include <UIAutomation.h>

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.interop.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>

#include "NativeModules.h"
#include "ReactPropertyBag.h"

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

struct CompReactPackageProvider
    : winrt::implements<CompReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    AddAttributedModules(packageBuilder, true);
  }
};

constexpr PCWSTR appName = L"RNTesterApp";

// Forward declarations of functions included in this code module:
winrt::Windows::Data::Json::JsonObject ListErrors(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Data::Json::JsonObject DumpVisualTree(winrt::Windows::Data::Json::JsonValue payload);
winrt::Windows::Foundation::IAsyncAction LoopServer(winrt::AutomationChannel::Server &server);

float ScaleFactor(HWND hwnd) noexcept {
  return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

// Create and configure the ReactNativeHost
winrt::Microsoft::ReactNative::ReactNativeHost CreateReactNativeHost(
    HWND hwnd,
    const winrt::Microsoft::UI::Composition::Compositor &compositor) {
  WCHAR workingDir[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, workingDir);

  auto host = winrt::Microsoft::ReactNative::ReactNativeHost();
  // Disable until we have a 3rd party story for custom components
  // RegisterAutolinkedNativeModulePackages(host.PackageProviders()); // Includes any
  // autolinked modules

  host.InstanceSettings().JavaScriptBundleFile(L"index.windows");
  host.InstanceSettings().DebugBundlePath(L"index");

  host.InstanceSettings().UseDirectDebugger(true);
  host.InstanceSettings().BundleRootPath(
      std::wstring(L"file:").append(workingDir).append(L"\\windows\\RNTesterApp-Fabric\\Bundle\\").c_str());
  host.InstanceSettings().DebuggerBreakOnNextLine(false);
#ifdef BUNDLE
  host.InstanceSettings().UseFastRefresh(true);
#endif
  // host.InstanceSettings().DebuggerPort(9229);
  host.InstanceSettings().UseDeveloperSupport(true);

  host.PackageProviders().Append(winrt::make<CompReactPackageProvider>());
  host.PackageProviders().Append(winrt::AutomationChannel::ReactPackageProvider());
  winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  // By using the MicrosoftCompositionContextHelper here, React Native Windows will use Lifted Visuals for its
  // tree.
  winrt::Microsoft::ReactNative::Composition::CompositionUIService::SetCompositionContext(
      host.InstanceSettings().Properties(),
      winrt::Microsoft::ReactNative::Composition::MicrosoftCompositionContextHelper::CreateContext(compositor));

  return host;
}

_Use_decl_annotations_ int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, PSTR /* commandLine */, int showCmd) {
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
  window.Title(appName);
  window.Resize({1000, 1000});
  window.Show();
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());

  auto host = CreateReactNativeHost(hwnd, compositor);

  // Nudge the ReactNativeHost to create the instance and wrapping context
  host.ReloadInstance();

  winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
  viewOptions.ComponentName(appName);

  auto compRootView = winrt::Microsoft::ReactNative::CompositionRootView(compositor);
  compRootView.ReactViewHost(winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));

  window.Changed([wkRootView = winrt::make_weak(compRootView)](
                     winrt::Microsoft::UI::Windowing::AppWindow const &window,
                     winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const &args) {
    if (args.DidSizeChange()) {
      if (auto rootView = wkRootView.get()) {
        auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
        winrt::Windows::Foundation::Size size{
            window.ClientSize().Width / ScaleFactor(hwnd), window.ClientSize().Height / ScaleFactor(hwnd)};
        rootView.Arrange(size);
        rootView.Size(size);
      }
    }
  });

  // Quit App when main window is closed
  window.Destroying([](winrt::Microsoft::UI::Windowing::AppWindow const &window, winrt::IInspectable const & /*args*/) {
    PostQuitMessage(0);
  });

  auto bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(compositor, window.Id());

  bridge.Connect(compRootView.Island());

  auto invScale = 1.0f / ScaleFactor(hwnd);
  compRootView.RootVisual().Scale({invScale, invScale, invScale});

  bridge.Show();

  compRootView.ScaleFactor(ScaleFactor(hwnd));
  compRootView.Size({window.ClientSize().Width / ScaleFactor(hwnd), window.ClientSize().Height / ScaleFactor(hwnd)});

  bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  // Set Up Servers for E2E Testing
  winrt::AutomationChannel::CommandHandler handler;
  handler.BindOperation(L"DumpVisualTree", DumpVisualTree);
  handler.BindOperation(L"ListErrors", ListErrors);

  auto server = winrt::AutomationChannel::Server(handler);
  auto asyncAction = LoopServer(server);

  dispatcherQueueController.DispatcherQueue().RunEventLoop();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  dispatcherQueueController.ShutdownQueue();

  // Destroy all Composition objects
  compositor.Close();
  compositor = {nullptr};
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

winrt::Windows::Data::Json::JsonObject DumpVisualTree(winrt::Windows::Data::Json::JsonValue payload) {
  winrt::Windows::Data::Json::JsonObject result;
  // TODO: Method should return a JSON of the Composition Visual Tree
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
