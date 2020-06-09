#pragma once

#include "winrt/Microsoft.ReactNative.h"

// clang-format off
namespace winrt::{{ namespace }}::implementation
{
    struct ReactPackageProvider : winrt::implements<ReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider>
    {
    public: // IReactPackageProvider
        void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;
    };
} // namespace winrt::{{ namespace }}::implementation

// clang-format on
