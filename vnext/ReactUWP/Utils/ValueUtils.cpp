// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/ValueUtils.h>
#include "unicode.h"

#include <folly/dynamic.h>
#include <iomanip>

#pragma warning( push )
#pragma warning( disable : 4995 )
#include <locale>
#pragma warning( pop )

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
}

namespace react { namespace uwp {

inline BYTE GetAFromArgb(DWORD v) { return ((BYTE)((v & 0xFF000000) >> 24)); }
inline BYTE GetRFromArgb(DWORD v) { return ((BYTE)((v & 0x00FF0000) >> 16)); }
inline BYTE GetGFromArgb(DWORD v) { return ((BYTE)((v & 0x0000FF00) >> 8)); }
inline BYTE GetBFromArgb(DWORD v) { return ((BYTE)((v & 0x000000FF))); }

struct ColorComp
{
  bool operator()(const winrt::Color& lhs, const winrt::Color& rhs) const
  {
    return
      (lhs.A < rhs.A || lhs.A == rhs.A &&
       (lhs.R < rhs.R || lhs.R == rhs.R &&
       (lhs.G < rhs.G || lhs.G == rhs.G &&
        lhs.B < rhs.B)));
  }
};

REACTWINDOWS_API_(winrt::Color) ColorFrom(const folly::dynamic& d)
{
  UINT argb = static_cast<UINT>(d.getInt());
  return winrt::ColorHelper::FromArgb(GetAFromArgb(argb), GetRFromArgb(argb), GetGFromArgb(argb), GetBFromArgb(argb));
}

REACTWINDOWS_API_(winrt::SolidColorBrush) SolidColorBrushFrom(const folly::dynamic& d)
{
  thread_local static std::map<winrt::Color, winrt::weak_ref<winrt::SolidColorBrush>, ColorComp> solidColorBrushCache;

  const auto color = ColorFrom(d);
  if (solidColorBrushCache.count(color) != 0)
  {
    auto brush = solidColorBrushCache[color].get();
    if (brush != nullptr)
    {
      return brush;
    }
  }

  winrt::SolidColorBrush brush(color);
  solidColorBrushCache[color] = winrt::make_weak(brush);
  return brush;
}

REACTWINDOWS_API_(winrt::Brush) BrushFrom(const folly::dynamic& d)
{
  return SolidColorBrushFrom(d);
}

REACTWINDOWS_API_(winrt::HorizontalAlignment) HorizontalAlignmentFrom(const folly::dynamic& d)
{
  auto valueString = d.asString();
  if (valueString == "center")
    return winrt::HorizontalAlignment::Center;
  else if (valueString == "left")
    return winrt::HorizontalAlignment::Left;
  else if (valueString == "right")
    return winrt::HorizontalAlignment::Right;
  else if (valueString == "stretch")
    return winrt::HorizontalAlignment::Stretch;

  // ASSERT: Invalid value for VerticalAlignment. Shouldn't get this far.
  assert(false);
  return winrt::HorizontalAlignment::Stretch;
}

REACTWINDOWS_API_(winrt::VerticalAlignment) VerticalAlignmentFrom(const folly::dynamic& d)
{
  auto valueString = d.asString();
  if (valueString == "bottom")
    return winrt::VerticalAlignment::Bottom;
  else if (valueString == "center")
    return winrt::VerticalAlignment::Center;
  else if (valueString == "stretch")
    return winrt::VerticalAlignment::Stretch;
  else if (valueString == "top")
    return winrt::VerticalAlignment::Top;

  // ASSERT: Invalid value for VerticalAlignment. Shouldn't get this far.
  assert(false);
  return winrt::VerticalAlignment::Stretch;
}

REACTWINDOWS_API_(winrt::DateTime) DateTimeFrom(int64_t timeInMilliSeconds, int64_t timeZoneOffsetInSeconds)
{
  auto timeInSeconds = (int64_t)timeInMilliSeconds / 1000;
  time_t ttWithTimeZoneOffset = (time_t)(timeInSeconds + timeZoneOffsetInSeconds);
  winrt::DateTime dateTime = winrt::clock::from_time_t(ttWithTimeZoneOffset);

  return dateTime;
}

REACTWINDOWS_API_(folly::dynamic) DateTimeToDynamic(winrt::DateTime dateTime, int64_t timeZoneOffsetInSeconds)
{
  time_t ttInSeconds = winrt::clock::to_time_t(dateTime);
  auto timeInUtc = ttInSeconds - timeZoneOffsetInSeconds;
  auto ttInMilliseconds = (int64_t)timeInUtc * 1000;
  auto strDateTime = folly::dynamic(std::to_string(ttInMilliseconds));

  return strDateTime;
}

REACTWINDOWS_API_(std::wstring) asWStr(const folly::dynamic& d)
{
  return facebook::react::unicode::Utf8ToUtf16(d.getString());
}

REACTWINDOWS_API_(folly::dynamic) HstringToDynamic(winrt::hstring hstr)
{
  return folly::dynamic(facebook::react::unicode::Utf16ToUtf8(hstr.c_str(), hstr.size()));
}

REACTWINDOWS_API_(winrt::hstring) asHstring(const folly::dynamic& d)
{
  return winrt::hstring(asWStr(d));
}

REACTWINDOWS_API_(winrt::TimeSpan) TimeSpanFromMs(double ms)
{
  std::chrono::milliseconds dur((int64_t)ms);
  return winrt::TimeSpan::duration(dur);
}

} }
