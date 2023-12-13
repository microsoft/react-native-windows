#pragma once
#include "PlaygroundReactPackageProvider.g.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::PlaygroundNativeModules::implementation {
struct PlaygroundReactPackageProvider : PlaygroundReactPackageProviderT<PlaygroundReactPackageProvider> {
  PlaygroundReactPackageProvider() = default;

  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
};
} // namespace winrt::PlaygroundNativeModules::implementation

namespace winrt::PlaygroundNativeModules::factory_implementation {

struct PlaygroundReactPackageProvider
    : PlaygroundReactPackageProviderT<PlaygroundReactPackageProvider, implementation::PlaygroundReactPackageProvider> {
};

} // namespace winrt::PlaygroundNativeModules::factory_implementation
