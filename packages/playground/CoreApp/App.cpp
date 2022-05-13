#include "pch.h"
#include "CoreApp.h"

// This app demonstrates the usage of the Microsoft.ReactNative CoreApp APIs
// use the preprocessor defines WITH_MODULES (true/false), FROM_JSON (true/false),
// or neither to exercise the RNStartCoreAppWithModules, RNStartCoreApp, and RNStartCoreAppFromConfigJson APIs.

#ifdef WITH_MODULES
#include <ModuleRegistration.h>
#include <NativeModules.h>
#include <winrt/Microsoft.ReactNative.h>

REACT_MODULE(LogBox)
struct LogBox {
  REACT_INIT(Initialize)
  void Initialize(const winrt::Microsoft::ReactNative::ReactContext &) noexcept {
    MessageBox(nullptr, L"Native Modules work!", L"Success", MB_OK);
  }
};

struct ThisAppPackageProvider
    : winrt::implements<ThisAppPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    for (auto const *reg = winrt::Microsoft::ReactNative::ModuleRegistration::Head(); reg != nullptr;
         reg = reg->Next()) {
      packageBuilder.AddModule(reg->ModuleName(), reg->MakeModuleProvider());
    }
  }
};
#endif

#ifdef FROM_JSON
int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR /*server*/, _In_ int /*showCommand*/) {
  RNStartCoreAppFromConfigJson(L"app.config.json");
}
#else
int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR /*server*/, _In_ int /*showCommand*/) {

#ifdef WITH_MODULES
  RNStartCoreAppWithModules(
      [](RNCoreApp *app) {
        app->componentName = L"RNTesterApp";
        app->jsBundleFile = LR"(Samples\rntester)";
        app->useDeveloperSupport = true;
        app->useWebDebugger = false;
      },
      [](RNCoreApp *app) {
        app->packageProvidersAbi = new void *[1];
        app->packageProvidersAbiCount = 1;
        auto provider = winrt::make<ThisAppPackageProvider>();
        app->packageProvidersAbi[0] = nullptr;
        winrt::copy_to_abi(provider, app->packageProvidersAbi[0]);
      });
#else
  RNStartCoreApp([](RNCoreApp *app) {
    app->componentName = L"RNTesterApp";
    app->jsBundleFile = LR"(Samples\rntester)";
    app->useDeveloperSupport = true;
    app->useWebDebugger = false;
  });
#endif
}
#endif
