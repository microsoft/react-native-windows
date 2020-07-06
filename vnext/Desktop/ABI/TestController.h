#pragma once
#include "Microsoft.Internal.TestController.g.h"

namespace winrt::Microsoft::Internal::implementation {
struct TestController {
  TestController() = default;

  static hstring HelloWorld();
};
} // namespace winrt::Microsoft::Internal::implementation

namespace winrt::Microsoft::Internal::factory_implementation {
struct TestController : TestControllerT<TestController, implementation::TestController> {};
} // namespace winrt::Microsoft::Internal::factory_implementation
