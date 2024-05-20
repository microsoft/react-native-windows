// SampleAppFabric.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "SampleAppFabric.h"

#include "AutolinkedNativeModules.g.h"

#include "NativeModules.h"

namespace winrt::ReactNative {
using namespace winrt::Microsoft::ReactNative;
}

namespace winrt::UI {
using namespace winrt::Microsoft::UI;
}

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

float ScaleFactor(HWND hwnd) noexcept {
  return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

void UpdateRootViewSizeToAppWindow(
    winrt::Microsoft::ReactNative::CompositionRootView const &rootView,
    winrt::Microsoft::UI::Windowing::AppWindow const &window) {
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
  auto scaleFactor = ScaleFactor(hwnd);
  winrt::Windows::Foundation::Size size{
      window.ClientSize().Width / scaleFactor, window.ClientSize().Height / scaleFactor};
  // Do not relayout when minimized
  if (window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().State() !=
      winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Minimized) {
    rootView.Arrange(size);
    rootView.Size(size);
  }
}

winrt::ReactNative::ReactApplicationInstanceSettings SetInstanceSettings() {
  WCHAR appDirectory[MAX_PATH];
  GetModuleFileNameW(NULL, appDirectory, MAX_PATH);
  PathCchRemoveFileSpec(appDirectory, MAX_PATH);

  winrt::ReactNative::ReactApplicationInstanceSettings instanceSettings{
      std::wstring(L"file://").append(appDirectory).append(L"\\Bundle\\").c_str()};

  return instanceSettings;
}

_Use_decl_annotations_ int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, PSTR /* commandLine */, int showCmd) {
  // Initialize WinRT.
  winrt::init_apartment(winrt::apartment_type::single_threaded);

  // Enable per monitor DPI scaling
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  // Create a DispatcherQueue for this thread.  This is needed for Composition, Content, and
  // Input APIs.
  auto dispatcherQueueController{winrt::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread()};

  // Create a Compositor for all Content on this thread.
  auto compositor{winrt::UI::Composition::Compositor()};

  // Create a top-level window.
  auto window = winrt::UI::Windowing::AppWindow::Create();
  window.Title(windowTitle);
  window.Resize({1000, 1000});
  window.Show();
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
  auto scaleFactor = ScaleFactor(hwnd);

  // Settings for the host
  auto instanceSettings{SetInstanceSettings()};

  // Create a ReactNativeHost component
  auto reactNativeHost =
      winrt::ReactNative::ReactNativeWindow::CreateReactNativeHost(instanceSettings, window, compositor);

  // Include any autolinked modules
  RegisterAutolinkedNativeModulePackages(reactNativeHost.PackageProviders());

  reactNativeHost.PackageProviders().Append(winrt::make<CompReactPackageProvider>());

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle
  reactNativeHost.ReloadInstance();

  // Create a RootView which will present a react-native component
  auto reactNativeIsland = winrt::ReactNative::ReactNativeWindow::CreateReactNativeIsland(
      compositor, window, reactNativeHost, mainComponentName);

  auto rootView = reactNativeIsland.RootView();

  // Update the size of the RootView when the AppWindow changes size
  window.Changed(
      [wkRootView = winrt::make_weak(rootView)](
          winrt::UI::Windowing::AppWindow const &window, winrt::UI::Windowing::AppWindowChangedEventArgs const &args) {
        if (args.DidSizeChange() || args.DidVisibilityChange()) {
          if (auto rootView = wkRootView.get()) {
            UpdateRootViewSizeToAppWindow(rootView, window);
          }
        }
      });

  // Quit application when main window is closed
  window.Destroying(
      [reactNativeHost](winrt::UI::Windowing::AppWindow const &window, winrt::IInspectable const & /*args*/) {
        // Before we shutdown the application - unload the ReactNativeHost to give the javascript a chance to save any
        // state
        auto async = reactNativeHost.UnloadInstance();
        async.Completed([reactNativeHost](auto asyncInfo, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
          assert(asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed);
          reactNativeHost.InstanceSettings().UIDispatcher().Post([]() { PostQuitMessage(0); });
        });
      });

  // DesktopChildSiteBridge create a ContentSite that can host the RootView ContentIsland
  auto bridge = winrt::UI::Content::DesktopChildSiteBridge::Create(compositor, window.Id());
  bridge.Connect(rootView.Island());
  bridge.ResizePolicy(winrt::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  rootView.ScaleFactor(scaleFactor);

  // Set the intialSize of the root view
  UpdateRootViewSizeToAppWindow(rootView, window);

  bridge.Show();

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
