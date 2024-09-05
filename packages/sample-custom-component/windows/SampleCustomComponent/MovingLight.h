#pragma once

#include "pch.h"
#include "resource.h"

#include "NativeModules.h"

namespace winrt::SampleCustomComponent {

void RegisterMovingLightNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;

} // namespace winrt::SampleCustomComponent
