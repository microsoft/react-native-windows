#include "pch.h"

#include <NativeModules.h>

using namespace React;

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
