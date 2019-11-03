#pragma once

#include "winrt/Microsoft.ReactNative.Bridge.h"

// clang-format off

using namespace winrt::Microsoft::ReactNative::Bridge;

namespace winrt::<%=ns%>::implementation
{

    struct ReactPackageProvider : winrt::implements<ReactPackageProvider, IReactPackageProvider>
    {
    public: // IReactPackageProvider
        void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
    };

} // namespace winrt::<%=ns%>::implementation

// clang-format on
