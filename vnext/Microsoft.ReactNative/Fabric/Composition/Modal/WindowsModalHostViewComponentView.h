// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

void RegisterWindowsModalHostNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
