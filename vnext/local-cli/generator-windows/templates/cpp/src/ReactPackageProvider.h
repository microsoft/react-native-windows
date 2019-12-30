#pragma once

#include "winrt/Microsoft.ReactNative.h"

// clang-format off

using namespace winrt::Microsoft::ReactNative;

namespace winrt::<%=ns%>::implementation
{

    struct ReactPackageProvider : winrt::implements<ReactPackageProvider, IReactPackageProvider>
    {
    public: // IReactPackageProvider
        void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
    };

} // namespace winrt::<%=ns%>::implementation

// clang-format on
