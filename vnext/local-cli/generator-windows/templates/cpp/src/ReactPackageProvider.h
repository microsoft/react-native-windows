#pragma once

#include "winrt/Microsoft.ReactNative.h"

// clang-format off
namespace winrt::<%=ns%>::implementation
{
    struct ReactPackageProvider : winrt::implements<ReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider>
    {
    public: // IReactPackageProvider
        void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;
    };
} // namespace winrt::<%=ns%>::implementation

// clang-format on
