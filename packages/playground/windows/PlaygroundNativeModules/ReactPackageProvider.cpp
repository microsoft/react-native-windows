#include "pch.h"
#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif
#include "GridItemViewManager.h"
#include "GridViewManager.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::PlaygroundNativeModules::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  packageBuilder.AddViewManager(L"GridViewManager", []() { return winrt::make<GridViewManager>(); });
  packageBuilder.AddViewManager(L"GridItemViewManager", []() { return winrt::make<GridItemViewManager>(); });
}

} // namespace winrt::PlaygroundNativeModules::implementation
