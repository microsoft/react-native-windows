#include "pch.h"
#include "ReactPackageProvider.h"
#include "NativeModules.h"

// clang-format off
using namespace winrt::Microsoft::ReactNative;

namespace winrt::{{ namespaceCpp }}::implementation
{

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept
{
    AddAttributedModules(packageBuilder);
}

} // namespace winrt::{{ namespaceCpp }}::implementation

// clang-format on
