#include "pch.h"
#include "PlaygroundReactPackageProvider.h"
#if __has_include("PlaygroundReactPackageProvider.g.cpp")
#include "PlaygroundReactPackageProvider.g.cpp"
#endif
#include "GridItemViewManager.h"
#include "GridViewManager.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::PlaygroundNativeModules::implementation {

void PlaygroundReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  packageBuilder.AddViewManager(L"GridViewManager", []() { return winrt::make<GridViewManager>(); });
  packageBuilder.AddViewManager(L"GridItemViewManager", []() { return winrt::make<GridItemViewManager>(); });
}

} // namespace winrt::PlaygroundNativeModules::implementation
