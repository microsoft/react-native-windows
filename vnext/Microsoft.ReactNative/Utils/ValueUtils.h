// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>

#include <Shared/ReactWindowsAPI.h>
#include "CppWinRTIncludes.h"

namespace folly {
struct dynamic;
}

namespace winrt::Microsoft::ReactNative {
struct JSValue;
}

namespace react::uwp {

xaml::Media::Brush BrushFromColorObject(const folly::dynamic &d);
xaml::Media::Brush BrushFromColorObject(const winrt::Microsoft::ReactNative::JSValue &v);
xaml::Media::SolidColorBrush SolidBrushFromColor(winrt::Windows::UI::Color color);

REACTWINDOWS_API_(winrt::Windows::UI::Color) ColorFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::UI::Color) ColorFrom(const winrt::Microsoft::ReactNative::JSValue &d);
REACTWINDOWS_API_(xaml::Media::Brush)
BrushFrom(const folly::dynamic &d);

REACTWINDOWS_API_(xaml::Media::Brush)
BrushFrom(const winrt::Microsoft::ReactNative::JSValue &v);

REACTWINDOWS_API_(xaml::Media::SolidColorBrush)
SolidColorBrushFrom(const folly::dynamic &d);
REACTWINDOWS_API_(xaml::Media::SolidColorBrush)
SolidColorBrushFrom(const winrt::Microsoft::ReactNative::JSValue &v);
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
winrt::hstring asHstring(const winrt::Microsoft::ReactNative::JSValue &v);
REACTWINDOWS_API_(folly::dynamic) HstringToDynamic(winrt::hstring hstr);
REACTWINDOWS_API_(bool) IsValidColorValue(const folly::dynamic &d);
REACTWINDOWS_API_(bool)
IsValidColorValue(const winrt::Microsoft::ReactNative::JSValue &v);

REACTWINDOWS_API_(winrt::Windows::Foundation::TimeSpan)
TimeSpanFromMs(double ms);

winrt::Uri UriTryCreate(winrt::param::hstring const &uri);

} // namespace react::uwp
