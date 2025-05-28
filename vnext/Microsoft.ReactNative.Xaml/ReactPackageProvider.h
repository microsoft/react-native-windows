#pragma once
#include "ReactPackageProvider.g.h"

namespace winrt::Microsoft::ReactNative::Xaml::implementation {
struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider> {
  ReactPackageProvider() = default;

  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder);
};
} // namespace winrt::Microsoft::ReactNative::Xaml::implementation
namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation {
struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider, implementation::ReactPackageProvider> {};
} // namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation
