// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/ValueUtils.h>
#include "UnicodeConversion.h"

#include <folly/dynamic.h>
#include <iomanip>

#pragma warning( push )
#pragma warning( disable : 4995 )
#include <locale>
#pragma warning( pop )

namespace winrt {
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

const auto dateTimeFormatISO8601 = "%Y-%m-%dT%H:%M:%S";

REACTWINDOWS_API_(winrt::Color) ColorFrom(const folly::dynamic& d)
{
  UINT argb = static_cast<UINT>(d.getInt());
  return winrt::ColorHelper::FromArgb(GetAFromArgb(argb), GetRFromArgb(argb), GetGFromArgb(argb), GetBFromArgb(argb));
}

REACTWINDOWS_API_(winrt::SolidColorBrush) SolidColorBrushFrom(const folly::dynamic& d)
{
  return winrt::SolidColorBrush(ColorFrom(d));
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

REACTWINDOWS_API_(winrt::Windows::Foundation::DateTime) DateTimeFrom(const folly::dynamic& d)
{
  winrt::Windows::Foundation::DateTime dateTime;
  std::tm tm;
  std::stringstream dateTimeStream(d.asString());
  dateTimeStream >> std::get_time(&tm, dateTimeFormatISO8601);
  
  if (!dateTimeStream.fail())
    dateTime = winrt::clock::from_time_t(std::mktime(&tm));

  return dateTime;
}

REACTWINDOWS_API_(folly::dynamic) DateTimeToDynamic(winrt::Windows::Foundation::DateTime dateTime)
{
  folly::dynamic strDateTime;
  char buff[100];
  struct tm newtime;
  time_t tt = winrt::clock::to_time_t(dateTime);
  localtime_s(&newtime, &tt);
  auto isformatted = strftime(buff, sizeof(buff), dateTimeFormatISO8601, &newtime);

  if (isformatted)
    strDateTime = folly::dynamic(std::string(buff));

  return strDateTime;
}

REACTWINDOWS_API_(std::wstring) asWStr(const folly::dynamic& d)
{
  return facebook::react::UnicodeConversion::Utf8ToUtf16(d.getString());
}

REACTWINDOWS_API_(folly::dynamic) HstringToDynamic(winrt::hstring hstr)
{
  return folly::dynamic(facebook::react::UnicodeConversion::Utf16ToUtf8(hstr.c_str(), hstr.size()));
}

REACTWINDOWS_API_(winrt::hstring) asHstring(const folly::dynamic& d)
{
  return winrt::hstring(asWStr(d));
}

REACTWINDOWS_API_(winrt::Windows::Foundation::TimeSpan) TimeSpanFromMs(double ms)
{
  std::chrono::milliseconds dur((int64_t)ms);
  return winrt::Windows::Foundation::TimeSpan::duration(dur);
}

} }
