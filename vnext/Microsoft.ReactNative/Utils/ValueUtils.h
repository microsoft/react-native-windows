// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>

#include <Shared/ReactWindowsAPI.h>
#include "CppWinRTIncludes.h"

namespace folly {
struct dynamic;
}

namespace react::uwp {

xaml::Media::Brush BrushFromColorObject(const folly::dynamic &d);

REACTWINDOWS_API_(winrt::Windows::UI::Color) ColorFrom(const folly::dynamic &d);
REACTWINDOWS_API_(xaml::Media::Brush)
BrushFrom(const folly::dynamic &d);
REACTWINDOWS_API_(xaml::Media::SolidColorBrush)
SolidColorBrushFrom(const folly::dynamic &d);
REACTWINDOWS_API_(xaml::VerticalAlignment)
VerticalAlignmentFrom(const folly::dynamic &d);
REACTWINDOWS_API_(xaml::HorizontalAlignment)
HorizontalAlignmentFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::Foundation::DateTime)
DateTimeFrom(int64_t timeInMilliSeconds, int64_t timeZoneOffsetInSeconds);
REACTWINDOWS_API_(folly::dynamic)
DateTimeToDynamic(winrt::Windows::Foundation::DateTime dateTime, int64_t timeZoneOffsetInSeconds);

REACTWINDOWS_API_(std::wstring) asWStr(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::hstring) asHstring(const folly::dynamic &d);
REACTWINDOWS_API_(folly::dynamic) HstringToDynamic(winrt::hstring hstr);
REACTWINDOWS_API_(bool) IsValidColorValue(const folly::dynamic &d);

REACTWINDOWS_API_(winrt::Windows::Foundation::TimeSpan)
TimeSpanFromMs(double ms);

} // namespace react::uwp
