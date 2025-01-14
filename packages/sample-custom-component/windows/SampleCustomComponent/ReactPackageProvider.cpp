// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "DrawingIsland.h"
#include "MovingLight.h"
#include "CalendarView.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::SampleCustomComponent::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder, true);

#ifdef RNW_NEW_ARCH
  RegisterDrawingIslandComponentView(packageBuilder);
  RegisterMovingLightNativeComponent(packageBuilder);
#endif // #ifdef RNW_NEW_ARCH

#if defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
  RegisterCalendarViewComponentView(packageBuilder);
#endif // #if defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
}

} // namespace winrt::SampleCustomComponent::implementation
