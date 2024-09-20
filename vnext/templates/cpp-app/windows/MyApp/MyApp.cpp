// {{ name }}.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "{{ name }}.h"

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
constexpr PCWSTR windowTitle = L"{{ mainComponentName }}";
constexpr PCWSTR mainComponentName = L"{{ mainComponentName }}";

_Use_decl_annotations_ int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, PSTR /* commandLine */, int showCmd) {
  // Initialize WinRT.
  winrt::init_apartment(winrt::apartment_type::single_threaded);

  // Enable per monitor DPI scaling
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  WCHAR appDirectory[MAX_PATH];
  GetModuleFileNameW(NULL, appDirectory, MAX_PATH);
  PathCchRemoveFileSpec(appDirectory, MAX_PATH);

  // DebugBundlePath is used when loading from a bundle server such as metro. If this parameter
  // is not provided fallback to a combination of JavaScriptBundleFile and BundleRootPath
  auto reactInstanceSettingsBuilder {
    winrt::Microsoft::ReactNative::ReactInstanceSettingsBuilder()
#if BUNDLE
        .BundleRootPath(appDirectory)
        .JavaScriptBundleFile(L"index.windows")
        .UseFastRefresh(false)
#else
        .JavaScriptBundleFile(L"index")
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
                                 .SetReactViewOptions(viewOptions)};

  // Start the react-native instance by creating a javascript runtime and load the bundle.
  auto reactNativeWin32App{reactNativeAppBuilder.Build()};

  auto window{reactNativeWin32App.AppWindow()};
  window.Title(windowTitle);

  // Start the island in the application.
  reactNativeWin32App.Start();
}
