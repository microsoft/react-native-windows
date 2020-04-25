// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/ReactWindowsAPI.h>
#include "CppWinRTIncludes.h"
namespace react {
namespace uwp {

REACTWINDOWS_API_(void)
AnnounceLiveRegionChangedIfNeeded(const winrt::Windows::UI::Xaml::FrameworkElement &element);

REACTWINDOWS_API_(bool)
HasDynamicAutomationProperties(const winrt::Windows::UI::Xaml::UIElement &element);
} // namespace uwp
} // namespace react