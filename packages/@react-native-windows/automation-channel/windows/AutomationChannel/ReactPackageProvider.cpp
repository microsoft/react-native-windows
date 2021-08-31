#include "pch.h"
#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

using namespace winrt::Microsoft::ReactNative;

namespace winrt::AutomationChannel::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const & /*packageBuilder*/) noexcept {
  // Noop
}

} // namespace winrt::AutomationChannel::implementation
