#include "pch.h"
#include "ReactPackageProvider.h"
#include "ReactPackageProvider.g.cpp"
#include "ModuleRegistration.h"

#include "XamlHost.h"

namespace winrt::Microsoft::ReactNative::Xaml::implementation {
void ReactPackageProvider::CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  AddAttributedModules(packageBuilder, true);

  RegisterXamlHostComponentView(packageBuilder);
  RegisterXamlControl(packageBuilder);
}
} // namespace winrt::Microsoft::ReactNative::Xaml::implementation
