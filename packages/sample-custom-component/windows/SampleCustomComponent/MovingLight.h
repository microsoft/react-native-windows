// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "pch.h"
#include "resource.h"

#include "NativeModules.h"

#include "codegen/react/components/SampleCustomComponent/MovingLight.g.h"

#ifdef RNW_NEW_ARCH

namespace winrt::SampleCustomComponent {

void RegisterMovingLightNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;

} // namespace winrt::SampleCustomComponent

#endif // #ifdef RNW_NEW_ARCH
