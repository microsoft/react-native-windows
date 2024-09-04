#include "pch.h"

#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "DrawingIsland.h"
#include "MovingLight.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::SampleCustomComponent::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder, true);

  RegisterDrawingIslandComponentView(packageBuilder);
  RegisterMovingLightNativeComponent(packageBuilder);
}

} // namespace winrt::SampleCustomComponent::implementation
