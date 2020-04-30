#include "pch.h"

#include <winrt/Microsoft.ReactNative.h>

namespace ReactNativeIntegrationTests {

TEST_CLASS (ReactNativeHostTests) {
  TEST_METHOD(Activation_Succeeds) {
    try {
      winrt::Microsoft::ReactNative::ReactNativeHost host{};
      TestCheck(true);
    } catch (...) {
      TestCheckFail();
    }
  }
};

} // namespace ReactNativeIntegrationTests
