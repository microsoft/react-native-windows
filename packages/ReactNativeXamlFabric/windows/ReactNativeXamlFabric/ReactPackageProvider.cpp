#include "pch.h"

#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "ReactNativeXamlFabric.h"

#include "XamlHost.h"

#if !defined(RNW_NEW_ARCH) || !defined(USE_EXPERIMENTAL_WINUI3)
static_assert(false, "This module requires RNW_NEW_ARCH and USE_EXPERIMENTAL_WINUI3 to be defined.");
#endif

using namespace winrt::Microsoft::ReactNative;

namespace winrt::ReactNativeXamlFabric::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder, true);

  RegisterXamlHostComponentView(packageBuilder);
  RegisterXamlControl(packageBuilder);
}

} // namespace winrt::ReactNativeXamlFabric::implementation
