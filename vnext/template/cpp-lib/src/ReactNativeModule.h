#pragma once

#include "JSValue.h"
#include "NativeModules.h"

// clang-format off

using namespace winrt::Microsoft::ReactNative;

namespace winrt::{{ namespaceCpp }}
{

REACT_MODULE(ReactNativeModule)
struct ReactNativeModule
{
    REACT_CONSTANT(const1)
    const bool const1 = true;

    REACT_METHOD(method1)
    void method1() noexcept
    {

    }

};

} // namespace winrt::{{ namespaceCpp }}

// clang-format on
