// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Microsoft.Internal.TestController.g.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::Internal::implementation {
struct TestController {
  TestController() = default;

  static Microsoft::ReactNative::IJSValueReader CreateDynamicReader(Microsoft::ReactNative::IJSValueWriter writer);
  static Microsoft::ReactNative::IJSValueWriter CreateDynamicWriter();
  static Microsoft::ReactNative::IReactContext CreateContext(
      const Microsoft::ReactNative::IReactPropertyBag &propertyBag,
      const Microsoft::ReactNative::IReactNotificationService &notificationService);
  static Microsoft::ReactNative::IReactModuleBuilder CreateReactModuleBuilder(
      Microsoft::ReactNative::IReactContext context);
  static Microsoft::ReactNative::IReactPackageBuilder CreateReactPackageBuilder();
  static Microsoft::ReactNative::IRedBoxErrorFrameInfo
  CreateRedBoxErrorFrameInfo(hstring file, hstring method, uint32_t line, uint32_t column);
  static Microsoft::ReactNative::IRedBoxErrorInfo CreateRedBoxErrorInfo(
      hstring message,
      uint32_t id,
      array_view<Microsoft::ReactNative::IRedBoxErrorFrameInfo const> callstack);
  static Microsoft::ReactNative::IReactNonAbiValue CreateNonAbiValue(int32_t value);
};
} // namespace winrt::Microsoft::Internal::implementation

namespace winrt::Microsoft::Internal::factory_implementation {
struct TestController : TestControllerT<TestController, implementation::TestController> {};
} // namespace winrt::Microsoft::Internal::factory_implementation
