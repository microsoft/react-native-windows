#include "pch.h"

#include "NativeModules.h"

// Have to use TurboModules to override built in modules.. so the standard attributed package provider doesn't work.
struct StubDeviceInfoReactPackageProvider
    : winrt::implements<StubDeviceInfoReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    AddAttributedModules(packageBuilder, true);
  }
};

winrt::Microsoft::ReactNative::IReactPackageProvider CreateStubDeviceInfoPackageProvider() noexcept {
  return winrt::make<StubDeviceInfoReactPackageProvider>();
}
