#include "pch.h"
#include <CppUnitTest.h>

#include "ActivationFactory.h"

namespace ABITests {

TEST_CLASS (ReactNativeHostTests) {
 public:
  ReactNativeHostTests() noexcept {
    winrt_activation_handler = WINRT_RoGetActivationFactory;
  }

  TEST_METHOD(Activation_Succeeds) {
    try {
      winrt::Microsoft::ReactNative::ReactNativeHost host{};

      // TODO: success primitive?
      Assert::IsTrue(true);
      Asser
    } catch (...) {
      // TODO: failure primitive?
      Assert::IsTrue(false);
    }
  }
};

} // namespace ABITests
