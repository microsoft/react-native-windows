#include "pch.h"

#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "ReactNativeXamlFabric.h"

#include "XamlHost.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::ReactNativeXamlFabric::implementation
{

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept
{
  AddAttributedModules(packageBuilder, true);

#if defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
  RegisterXamlHostComponentView(packageBuilder);
  RegisterXamlControl(packageBuilder);
#endif

}

} // namespace winrt::ReactNativeXamlFabric::implementation
