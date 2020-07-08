#pragma once
#include "Microsoft.Internal.TestController.g.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::Internal::implementation {
struct TestController {
  TestController() = default;

  static Microsoft::ReactNative::IJSValueReader CreateDynamicReader(Microsoft::ReactNative::IJSValueWriter writer);
  static Microsoft::ReactNative::IJSValueWriter CreateDynamicWriter();
};
} // namespace winrt::Microsoft::Internal::implementation

namespace winrt::Microsoft::Internal::factory_implementation {
struct TestController : TestControllerT<TestController, implementation::TestController> {};
} // namespace winrt::Microsoft::Internal::factory_implementation
