// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>

#include <Shared/ReactWindowsAPI.h>
#include "CppWinRTIncludes.h"

#include <react/renderer/graphics/Color.h>

namespace folly {
struct dynamic;
}

namespace winrt::Microsoft::ReactNative {
struct JSValue;
}

namespace Microsoft::ReactNative {

REACTWINDOWS_API_(winrt::Windows::UI::Color) ColorFrom(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::Windows::UI::Color) ColorFrom(const winrt::Microsoft::ReactNative::JSValue &v);

REACTWINDOWS_API_(std::wstring) asWStr(const folly::dynamic &d);
REACTWINDOWS_API_(winrt::hstring) asHstring(const folly::dynamic &d);
winrt::hstring asHstring(const winrt::Microsoft::ReactNative::JSValue &v);
REACTWINDOWS_API_(folly::dynamic) HstringToDynamic(winrt::hstring hstr);
REACTWINDOWS_API_(bool) IsValidColorValue(const folly::dynamic &d);
REACTWINDOWS_API_(bool)
IsValidColorValue(const winrt::Microsoft::ReactNative::JSValue &v);

winrt::Windows::UI::Color ColorFromNumber(DWORD argb) noexcept;

bool IsValidOptionalColorValue(const winrt::Microsoft::ReactNative::JSValue &v);
std::optional<winrt::Windows::UI::Color> OptionalColorFrom(const winrt::Microsoft::ReactNative::JSValue &v);

} // namespace Microsoft::ReactNative
