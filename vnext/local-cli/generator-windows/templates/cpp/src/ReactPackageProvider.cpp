#include "pch.h"
#include "ReactPackageProvider.h"
#include "NativeModules.h"

// clang-format off
using namespace winrt::Microsoft::ReactNative;

namespace winrt::{{ namespace }}::implementation
{

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept
{
    AddAttributedModules(packageBuilder);
}

} // namespace winrt::{{ namespace }}::implementation

// clang-format on
