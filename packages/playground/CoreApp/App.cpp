// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include <combaseapi.h>
#include "CoreApp.h"

// This app demonstrates the usage of the Microsoft.ReactNative CoreApp APIs
// use the preprocessor defines WITH_MODULES (true/false), FROM_JSON (true/false),
// or neither to exercise the RNCoreAppStart, and RNCoreAppStartFromConfigJson APIs.
#define DEFINE_MODULES

#define WITH_MODULES
// #define FROM_JSON

#ifdef DEFINE_MODULES
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
    winrt::Microsoft::ReactNative::AddAttributedModules(packageBuilder, true);
  }
};

extern "C" __declspec(dllexport) void *__cdecl MySpecialPackageProvider() {
  auto provider = winrt::make<ThisAppPackageProvider>();
  void *abi{nullptr};
  winrt::copy_to_abi(provider, abi);
  return abi;
}

#endif

#ifdef FROM_JSON
int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR /*server*/, _In_ int /*showCommand*/) {
  RNCoreAppStartFromConfigJson(L"app.config.json", nullptr, nullptr);
}
#else
int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR /*server*/, _In_ int /*showCommand*/) {
  RNCoreAppStart(
      [](RNCoreApp *app, void *) {
        app->componentName = L"RNTesterApp";
        app->jsBundleFile = LR"(Samples\rntester)";
        app->useDeveloperSupport = true;
        app->useWebDebugger = false;

#ifdef WITH_MODULES
        app->packageProvidersAbiCount = 1;
        app->packageProvidersAbi =
            reinterpret_cast<void **>(CoTaskMemAlloc(sizeof(void *) * app->packageProvidersAbiCount));
        app->packageProvidersAbi[0] = MySpecialPackageProvider();
#endif
      },
      nullptr);
}
#endif
