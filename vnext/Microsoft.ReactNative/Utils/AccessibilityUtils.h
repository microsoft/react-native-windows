// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/ReactWindowsAPI.h>
#include "CppWinRTIncludes.h"
namespace react {
namespace uwp {

REACTWINDOWS_API_(void)
AnnounceLiveRegionChangedIfNeeded(const xaml::FrameworkElement &element);

REACTWINDOWS_API_(bool)
HasDynamicAutomationProperties(const xaml::UIElement &element);
} // namespace uwp
} // namespace react
