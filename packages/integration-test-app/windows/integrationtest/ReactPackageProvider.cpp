#include "pch.h"
#include "ReactPackageProvider.h"
#include "NativeModules.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::integrationtest::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  // We do not use TurboModules for the TestModule, since the integration tests are specifically targeting
  // NativeModules NOT TurboModules.
  //
  // See here for an example usage of NativeModules.TestModule:
  // https://github.com/facebook/react-native/blob/153aedce413ef73f5e026abdfcf1346a37cec219/IntegrationTests/AppEventsTest.js#L16
  AddAttributedModules(packageBuilder, false);
}

} // namespace winrt::integrationtest::implementation
