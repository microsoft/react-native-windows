// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include "CoreAppPage.g.h"
#include <UI.Xaml.Markup.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct CoreAppPage : CoreAppPageT<CoreAppPage> {
  CoreAppPage();
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct CoreAppPage : CoreAppPageT<CoreAppPage, implementation::CoreAppPage> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
