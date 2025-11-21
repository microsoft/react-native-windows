// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <CppWinRTIncludes.h>
#include <Utils/ValueUtils.h>
#include <windows.foundation.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "Unicode.h"
#include "XamlUtils.h"

#include <JSValue.h>
#include <folly/dynamic.h>
#include <iomanip>

#include <locale>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Input;
using namespace xaml::Media;
} // namespace winrt

namespace Microsoft::ReactNative {

inline BYTE GetAFromArgb(DWORD v) {
  return ((BYTE)((v & 0xFF000000) >> 24));
}
inline BYTE GetRFromArgb(DWORD v) {
  return ((BYTE)((v & 0x00FF0000) >> 16));
}
inline BYTE GetGFromArgb(DWORD v) {
  return ((BYTE)((v & 0x0000FF00) >> 8));
}
inline BYTE GetBFromArgb(DWORD v) {
  return ((BYTE)((v & 0x000000FF)));
}

struct ColorComp {
  bool operator()(const winrt::Color &lhs, const winrt::Color &rhs) const {
    return (
        lhs.A < rhs.A ||
        lhs.A == rhs.A && (lhs.R < rhs.R || lhs.R == rhs.R && (lhs.G < rhs.G || lhs.G == rhs.G && lhs.B < rhs.B)));
  }
};

winrt::Color ColorFromNumber(DWORD argb) noexcept {
  return xaml::FromArgb(GetAFromArgb(argb), GetRFromArgb(argb), GetGFromArgb(argb), GetBFromArgb(argb));
}

REACTWINDOWS_API_(winrt::Color) ColorFrom(const folly::dynamic &d) {
  if (!d.isNumber())
    return winrt::Colors::Transparent();
  return ColorFromNumber(static_cast<UINT>(d.asInt()));
}

REACTWINDOWS_API_(winrt::Color) ColorFrom(const winrt::Microsoft::ReactNative::JSValue &v) {
  if (v.Type() != winrt::Microsoft::ReactNative::JSValueType::Int64 &&
      v.Type() != winrt::Microsoft::ReactNative::JSValueType::Double)
    return winrt::Colors::Transparent();
  return ColorFromNumber(v.AsUInt32());
}

REACTWINDOWS_API_(std::wstring) asWStr(const folly::dynamic &d) {
  return Microsoft::Common::Unicode::Utf8ToUtf16(d.getString());
}

REACTWINDOWS_API_(folly::dynamic) HstringToDynamic(winrt::hstring hstr) {
  return folly::dynamic(Microsoft::Common::Unicode::Utf16ToUtf8(hstr.c_str(), hstr.size()));
}

REACTWINDOWS_API_(winrt::hstring) asHstring(const folly::dynamic &d) {
  return winrt::hstring(asWStr(d));
}

winrt::hstring asHstring(const winrt::Microsoft::ReactNative::JSValue &v) {
  return winrt::hstring(Microsoft::Common::Unicode::Utf8ToUtf16(v.AsString()));
}

REACTWINDOWS_API_(bool) IsValidColorValue(const folly::dynamic &d) {
  return d.isObject() ? (d.find("windowsbrush") != d.items().end()) : d.isNumber();
}

REACTWINDOWS_API_(bool) IsValidColorValue(const winrt::Microsoft::ReactNative::JSValue &v) {
  if (v.Type() == winrt::Microsoft::ReactNative::JSValueType::Object) {
    return !v.AsObject()["windowsbrush"].IsNull();
  }
  return v.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
      v.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64;
}

bool IsValidOptionalColorValue(const winrt::Microsoft::ReactNative::JSValue &v) {
  return v.Type() == winrt::Microsoft::ReactNative::JSValueType::Null || IsValidColorValue(v);
}

std::optional<winrt::Color> OptionalColorFrom(const winrt::Microsoft::ReactNative::JSValue &v) {
  return v.IsNull() ? std::optional<winrt::Color>{} : ColorFrom(v);
}

} // namespace Microsoft::ReactNative
