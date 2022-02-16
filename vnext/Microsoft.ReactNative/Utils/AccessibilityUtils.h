// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Shared/ReactWindowsAPI.h>
#include "CppWinRTIncludes.h"
namespace Microsoft::ReactNative {

REACTWINDOWS_API_(void)
AnnounceLiveRegionChangedIfNeeded(const xaml::FrameworkElement &element);

REACTWINDOWS_API_(bool)
HasDynamicAutomationProperties(const xaml::UIElement &element);
} // namespace Microsoft::ReactNative
