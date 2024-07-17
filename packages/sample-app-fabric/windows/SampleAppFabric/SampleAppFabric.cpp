// SampleAppFabric.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "SampleAppFabric.h"

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
constexpr PCWSTR windowTitle = L"sample_app_fabric";
constexpr PCWSTR mainComponentName = L"sample_app_fabric";

// Scaling factor for the window's content based on the DPI of the display where the window is located.
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
    constraints.MaximumSize = constraints.MinimumSize = size;
    rootView.Arrange(constraints, {0, 0});
  }
}

_Use_decl_annotations_ int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, PSTR /* commandLine */, int showCmd) {
  // Initialize WinRT.
  winrt::init_apartment(winrt::apartment_type::single_threaded);

  // Enable per monitor DPI scaling
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  // Create a DispatcherQueue for this thread.  This is needed for Composition, Content, and
  // Input APIs.
  auto dispatcherQueueController{winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread()};

  // Create a top-level window.
  auto window = winrt::Microsoft::UI::Windowing::AppWindow::Create();
  window.Title(windowTitle);
  window.Resize({1000, 1000});
  window.Show();
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
  auto scaleFactor = ScaleFactor(hwnd);

  WCHAR appDirectory[MAX_PATH];
  GetModuleFileNameW(NULL, appDirectory, MAX_PATH);
  PathCchRemoveFileSpec(appDirectory, MAX_PATH);

  // DebugBundlePath is used when loading from a bundle server such as metro. If this parameter
  // is not provided fallback to a combination of JavaScriptBundleFile and BundleRootPath
  auto reactInstanceSettingsBuilder {
    winrt::Microsoft::ReactNative::ReactInstanceSettingsBuilder()
        .DebugBundlePath(L"index")
        .JavaScriptBundleFile(L"index.windows")
        .BundleRootPath(appDirectory)
#if BUNDLE
        .UseFastRefresh(false)
#else
        .UseFastRefresh(true)
#endif
#if _DEBUG
        .UseDirectDebugger(true)
        .UseDeveloperSupport(true)
#else
        .UseDirectDebugger(false)
        .UseDeveloperSupport(false)
#endif
  };

  auto packageProviders{winrt::single_threaded_vector<winrt::Microsoft::ReactNative::IReactPackageProvider>()};

  RegisterAutolinkedNativeModulePackages(packageProviders);
  packageProviders.Append(winrt::make<CompReactPackageProvider>());

  winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
  viewOptions.ComponentName(mainComponentName);

  // Initialize and Manage the ReactNativeHost
  auto reactNativeAppBuilder{winrt::Microsoft::ReactNative::ReactNativeAppBuilder()
                                 .AddPackageProviders(packageProviders)
                                 .SetReactInstanceSettings(reactInstanceSettingsBuilder.ReactInstanceSettings())
                                 .SetAppWindow(window)
                                 .SetReactViewOptions(viewOptions)};

  // Start the react-native instance by creating a javascript runtime and load the bundle.
  auto host{reactNativeAppBuilder.Start()};

  auto reactNativeIsland{reactNativeAppBuilder.ReactNativeIsland()};

  // Update the size of the RootView when the AppWindow changes size
  window.Changed([wkRootView = winrt::make_weak(reactNativeIsland)](
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
      [host](winrt::Microsoft::UI::Windowing::AppWindow const &window, winrt::IInspectable const & /*args*/) {
        // Before we shutdown the application - unload the ReactNativeHost to give the javascript a chance to save any
        // state
        auto async = host.UnloadInstance();
        async.Completed([host](auto asyncInfo, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
          assert(asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed);
          host.InstanceSettings().UIDispatcher().Post([]() { PostQuitMessage(0); });
        });
      });

  // DesktopChildSiteBridge associated with a ContentIsland
  auto bridge = reactNativeAppBuilder.DesktopChildSiteBridge();

  reactNativeIsland.ScaleFactor(scaleFactor);

  // Set the intialSize of the root view
  UpdateRootViewSizeToAppWindow(reactNativeIsland, window);

  bridge.Show();

  // Run the main application event loop
  dispatcherQueueController.DispatcherQueue().RunEventLoop();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  dispatcherQueueController.ShutdownQueue();
}
