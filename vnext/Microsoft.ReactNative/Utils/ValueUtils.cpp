// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Markup.h>
#include <Utils/ValueUtils.h>
#include "Unicode.h"

#include <folly/dynamic.h>
#include <iomanip>

#pragma warning(push)
#pragma warning(disable : 4995)
#include <locale>
#pragma warning(pop)

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Input;
using namespace xaml::Media;
} // namespace winrt

namespace react::uwp {

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

xaml::Media::Brush BrushFromColorObject(const folly::dynamic &d) {
  winrt::hstring resourceName{winrt::to_hstring(d.find("windowsbrush")->second.asString())};

  thread_local static std::map<winrt::hstring, winrt::weak_ref<xaml::Media::SolidColorBrush>> accentColorMap = {
      {L"SystemAccentColor", {nullptr}},
      {L"SystemAccentColorLight1", {nullptr}},
      {L"SystemAccentColorLight2", {nullptr}},
      {L"SystemAccentColorLight3", {nullptr}},
      {L"SystemAccentColorDark1", {nullptr}},
      {L"SystemAccentColorDark2", {nullptr}},
      {L"SystemAccentColorDark3", {nullptr}},
      {L"SystemListAccentLowColor", {nullptr}},
      {L"SystemListAccentMediumColor", {nullptr}},
      {L"SystemListAccentHighColor", {nullptr}}};

  if (accentColorMap.find(resourceName) != accentColorMap.end()) {
    if (auto brush = accentColorMap.at(resourceName).get()) {
      return brush;
    }

    winrt::hstring xamlString =
        L"<ResourceDictionary"
        L"    xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L"    xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
        L"  <SolidColorBrush"
        L"      x:Key='" +
        resourceName +
        L"'"
        L"      Color='{ThemeResource " +
        resourceName +
        "}' />"
        L"</ResourceDictionary>";

    auto dictionary{winrt::unbox_value<winrt::ResourceDictionary>(winrt::Markup::XamlReader::Load(xamlString))};

    auto brush{winrt::unbox_value<xaml::Media::SolidColorBrush>(dictionary.Lookup(winrt::box_value(resourceName)))};

    accentColorMap[resourceName] = winrt::make_weak(brush);

    return brush;
  }

  winrt::IInspectable resource{winrt::Application::Current().Resources().Lookup(winrt::box_value(resourceName))};

  return winrt::unbox_value<winrt::Brush>(resource);
}

REACTWINDOWS_API_(winrt::Color) ColorFrom(const folly::dynamic &d) {
  UINT argb = static_cast<UINT>(d.asInt());
  return winrt::ColorHelper::FromArgb(GetAFromArgb(argb), GetRFromArgb(argb), GetGFromArgb(argb), GetBFromArgb(argb));
}

REACTWINDOWS_API_(xaml::Media::SolidColorBrush)
SolidColorBrushFrom(const folly::dynamic &d) {
  if (d.isObject()) {
    return BrushFromColorObject(d).as<xaml::Media::SolidColorBrush>();
  }

  thread_local static std::map<winrt::Color, winrt::weak_ref<xaml::Media::SolidColorBrush>, ColorComp>
      solidColorBrushCache;

  const auto color = d.isNumber() ? ColorFrom(d) : winrt::Colors::Transparent();
  if (solidColorBrushCache.count(color) != 0) {
    if (auto brush = solidColorBrushCache[color].get()) {
      return brush;
    }
  }

  xaml::Media::SolidColorBrush brush(color);
  solidColorBrushCache[color] = winrt::make_weak(brush);
  return brush;
}

REACTWINDOWS_API_(winrt::Brush) BrushFrom(const folly::dynamic &d) {
  if (d.isObject()) {
    return BrushFromColorObject(d);
  }

  return SolidColorBrushFrom(d);
}

REACTWINDOWS_API_(xaml::HorizontalAlignment)
HorizontalAlignmentFrom(const folly::dynamic &d) {
  auto valueString = d.asString();
  if (valueString == "center")
    return xaml::HorizontalAlignment::Center;
  else if (valueString == "left")
    return xaml::HorizontalAlignment::Left;
  else if (valueString == "right")
    return xaml::HorizontalAlignment::Right;
  else if (valueString == "stretch")
    return xaml::HorizontalAlignment::Stretch;

  // ASSERT: Invalid value for VerticalAlignment. Shouldn't get this far.
  assert(false);
  return xaml::HorizontalAlignment::Stretch;
}

REACTWINDOWS_API_(xaml::VerticalAlignment)
VerticalAlignmentFrom(const folly::dynamic &d) {
  auto valueString = d.asString();
  if (valueString == "bottom")
    return xaml::VerticalAlignment::Bottom;
  else if (valueString == "center")
    return xaml::VerticalAlignment::Center;
  else if (valueString == "stretch")
    return xaml::VerticalAlignment::Stretch;
  else if (valueString == "top")
    return xaml::VerticalAlignment::Top;

  // ASSERT: Invalid value for VerticalAlignment. Shouldn't get this far.
  assert(false);
  return xaml::VerticalAlignment::Stretch;
}

REACTWINDOWS_API_(winrt::DateTime)
DateTimeFrom(int64_t timeInMilliSeconds, int64_t timeZoneOffsetInSeconds) {
  auto timeInSeconds = (int64_t)timeInMilliSeconds / 1000;
  time_t ttWithTimeZoneOffset = (time_t)(timeInSeconds + timeZoneOffsetInSeconds);
  winrt::DateTime dateTime = winrt::clock::from_time_t(ttWithTimeZoneOffset);

  return dateTime;
}

REACTWINDOWS_API_(folly::dynamic)
DateTimeToDynamic(winrt::DateTime dateTime, int64_t timeZoneOffsetInSeconds) {
  time_t ttInSeconds = winrt::clock::to_time_t(dateTime);
  auto timeInUtc = ttInSeconds - timeZoneOffsetInSeconds;
  auto ttInMilliseconds = (int64_t)timeInUtc * 1000;
  auto strDateTime = folly::dynamic(std::to_string(ttInMilliseconds));

  return strDateTime;
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

REACTWINDOWS_API_(bool) IsValidColorValue(const folly::dynamic &d) {
  return d.isObject() ? (d.find("windowsbrush") != d.items().end()) : d.isNumber();
}

REACTWINDOWS_API_(winrt::TimeSpan) TimeSpanFromMs(double ms) {
  std::chrono::milliseconds dur((int64_t)ms);
  return winrt::TimeSpan::duration(dur);
}

} // namespace react::uwp
