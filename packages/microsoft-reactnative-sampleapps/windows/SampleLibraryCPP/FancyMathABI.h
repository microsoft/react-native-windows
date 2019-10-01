// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "FancyMathABI.g.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;

namespace winrt::SampleLibraryCPP::implementation
{
    struct FancyMathABI : FancyMathABIT<FancyMathABI>
    {
        FancyMathABI() = default;

        // INativeModule

        hstring Name() {
          return L"FancyMathABI";
        }

        void Initialize();

        IMapView<hstring, IInspectable> Constants();
        IVectorView<MethodInfo> Methods();

       private:
        IMapView<hstring, IInspectable> m_constants{nullptr};
        IVectorView<MethodInfo> m_methods{nullptr};
        
    };
}

namespace winrt::SampleLibraryCPP::factory_implementation
{
    struct FancyMathABI : FancyMathABIT<FancyMathABI, implementation::FancyMathABI>
    {
    };
}
