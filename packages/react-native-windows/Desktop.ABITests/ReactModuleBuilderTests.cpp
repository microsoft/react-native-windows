// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Internal.h>
#include <winrt/Microsoft.ReactNative.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

// The tests here are a development staging artifact owed to the incremental buildup of the C++/WinRT-based ABI of
// the Win32 DLL. They (or their logical equivalent) should probably get rolled into other tests once C++/WinRT-based
// instance management becomes available.
TEST_CLASS (ReactModuleBuilderTests) {
 public:
  ReactModuleBuilderTests() {
    IReactPropertyBag propertyBag = ReactPropertyBagHelper::CreatePropertyBag();
    IReactNotificationService notificationService = ReactNotificationServiceHelper::CreateNotificationService();
    IReactContext context = Microsoft::Internal::TestController::CreateContext(propertyBag, notificationService);
    m_builder = Microsoft::Internal::TestController::CreateReactModuleBuilder(context);
  }

  TEST_METHOD(AddConstantProvider_IsCallable) {
    m_builder.AddConstantProvider([](Microsoft::ReactNative::IJSValueWriter const &constantWriter) {});
  }

  TEST_METHOD(AddInitializer_IsCallable) {
    m_builder.AddInitializer([](Microsoft::ReactNative::IReactContext const &reactContext) {});
  }

  TEST_METHOD(AddMethod_IsCallable) {
    m_builder.AddMethod(
        /* name */ L"MethodName",
        /* returnType */ Microsoft::ReactNative::MethodReturnType::Void,
        /* method */
        [](Microsoft::ReactNative::IJSValueReader const &inputReader,
           Microsoft::ReactNative::IJSValueWriter const &outputWriter,
           Microsoft::ReactNative::MethodResultCallback const &resolve,
           Microsoft::ReactNative::MethodResultCallback const &reject) {});
  }

  TEST_METHOD(AddSyncMethod_IsCallable) {
    m_builder.AddSyncMethod(
        /* name */ L"MethodName",
        /* method */
        [](Microsoft::ReactNative::IJSValueReader const &inputReader,
           Microsoft::ReactNative::IJSValueWriter const &outputWriter) {});
  }

 private:
  IReactModuleBuilder m_builder;
};

} // namespace ABITests