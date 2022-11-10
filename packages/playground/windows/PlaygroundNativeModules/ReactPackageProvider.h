#pragma once
#include "ReactPackageProvider.g.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::PlaygroundNativeModules::implementation {
struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider> {
  ReactPackageProvider() = default;

  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
};
} // namespace winrt::PlaygroundNativeModules::implementation

namespace winrt::PlaygroundNativeModules::factory_implementation {

struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider, implementation::ReactPackageProvider> {};

} // namespace winrt::PlaygroundNativeModules::factory_implementation
