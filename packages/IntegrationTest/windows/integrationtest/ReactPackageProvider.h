#pragma once

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::integrationtest::implementation {
struct ReactPackageProvider
    : winrt::implements<ReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;
};
} // namespace winrt::integrationtest::implementation
