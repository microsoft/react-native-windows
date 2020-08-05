// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Internal.h>
#include <winrt/Microsoft.ReactNative.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

// The tests here are a development staging artifact owed to the incremental buildup of the C++/WinRT-based ABI of
// the Win32 DLL. They (or their logical equivalent) should probably get rolled into other tests once C++/WinRT-based
// instance management becomes available.
TEST_CLASS (ReactPackageBuilderTests) {
 public:
  TEST_METHOD(AddModule_IsCallable) {
    IReactPackageBuilder builder = Microsoft::Internal::TestController::CreateReactPackageBuilder();
    builder.AddModule(
        /* moduleName*/ L"ModuleName",
        /* moduleProvider */ [](Microsoft::ReactNative::IReactModuleBuilder const &moduleBuilder) {
          return IInspectable{};
        });
  }
};

} // namespace ABITests
