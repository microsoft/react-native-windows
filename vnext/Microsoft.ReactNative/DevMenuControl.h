// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DevMenuControl.g.h"
#include <CppWinRTIncludes.h>
#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct DevMenuControl : DevMenuControlT<DevMenuControl> {
  DevMenuControl();
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct DevMenuControl : DevMenuControlT<DevMenuControl, implementation::DevMenuControl> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
