// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>
#include <string>

#include <ReactWindowsCore/ReactWindowsAPI.h>

namespace folly {
struct dynamic;
}

namespace react {
namespace uwp {

REACTWINDOWS_API_(winrt::Windows::UI::Color) ColorFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::UI::Xaml::Media::Brush)
BrushFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::UI::Xaml::Media::SolidColorBrush)
SolidColorBrushFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::UI::Xaml::VerticalAlignment)
VerticalAlignmentFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::UI::Xaml::HorizontalAlignment)
HorizontalAlignmentFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::Foundation::DateTime)
DateTimeFrom(int64_t timeInMilliSeconds, int64_t timeZoneOffsetInSeconds);
REACTWINDOWS_API_(folly::dynamic)
DateTimeToDynamic(
    winrt::Windows::Foundation::DateTime dateTime,
    int64_t timeZoneOffsetInSeconds);

REACTWINDOWS_API_(std::wstring) asWStr(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::hstring) asHstring(const folly::dynamic &d);
REACTWINDOWS_API_(folly::dynamic) HstringToDynamic(winrt::hstring hstr);
REACTWINDOWS_API_(bool) IsValidColorValue(const folly::dynamic& d);

REACTWINDOWS_API_(winrt::Windows::Foundation::TimeSpan)
TimeSpanFromMs(double ms);

} // namespace uwp
} // namespace react
