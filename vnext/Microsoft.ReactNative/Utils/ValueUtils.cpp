// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <CppWinRTIncludes.h>
#include <UI.Xaml.Markup.h>
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>
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

xaml::Media::Brush BrushFromTheme(winrt::hstring resourceName) {
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
  return brush;
}

struct BrushCache {
  std::map<winrt::hstring, xaml::Media::Brush> m_map;
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{nullptr};
  BrushCache() {
    m_map = {
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

    m_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
    auto dq = winrt::dispatching::DispatcherQueue::GetForCurrentThread();
    m_uiSettings.ColorValuesChanged([this, dq](auto &&sender, auto &&args) {
      dq.TryEnqueue([this]() {
        for (auto &entry : m_map) {
          winrt::IInspectable resource{winrt::Application::Current().Resources().Lookup(winrt::box_value(entry.first))};
          if (auto oldSCBrush = entry.second.try_as<xaml::Media::SolidColorBrush>()) {
            if (auto newSCBrush = resource.try_as<xaml::Media::SolidColorBrush>()) {
              oldSCBrush.Color(newSCBrush.Color());
            }
          }
          // Similar logic can be applied to copy Acrylic or Reveal brushes
          /*
          else if (auto oldAcBrush = entry.second.try_as<xaml::Media::AcrylicBrush>()) {
            if (auto newAcBrush = resource.try_as<xaml::Media::AcrylicBrush>()) {
              // ...
            }
          }
          */
        }
      });
    });
  }

  xaml::Media::Brush BrushFromResourceName(winrt::hstring resourceName) {
    if (m_map.find(resourceName) != m_map.end()) {
      if (auto brush = m_map.at(resourceName)) {
        return brush;
      }

      auto brush = BrushFromTheme(resourceName);
      return RegisterBrush(resourceName, brush);
    }

    const auto appResources{winrt::Application::Current().Resources()};
    const auto boxedResourceName{winrt::box_value(resourceName)};
    if (appResources.HasKey(boxedResourceName)) {
      winrt::IInspectable resource{appResources.Lookup(boxedResourceName)};

      if (auto brush = resource.try_as<xaml::Media::Brush>()) {
        return RegisterBrush(resourceName, brush);
      } else if (auto color = resource.try_as<winrt::Windows::UI::Color>()) {
        auto brush = xaml::Media::SolidColorBrush(color.value());
        return RegisterBrush(resourceName, brush);
      }
    }

    assert(false && "Resource is not a Color or Brush");
    return xaml::Media::SolidColorBrush(winrt::Colors::Transparent());
  }

  xaml::Media::Brush RegisterBrush(winrt::hstring resourceName, const xaml::Media::Brush &brush) {
    if (auto scb = brush.try_as<xaml::Media::SolidColorBrush>()) {
      auto scb2 = xaml::Media::SolidColorBrush{scb.Color()};
      m_map.emplace(resourceName, scb2);
      return scb2;
    } else {
      m_map.emplace(resourceName, brush);
      return brush;
    }
  }
};

xaml::Media::Brush BrushFromColorObject(winrt::hstring resourceName) {
  thread_local static BrushCache accentColorMap;

  return accentColorMap.BrushFromResourceName(resourceName);
}

xaml::Media::Brush BrushFromColorObject(const folly::dynamic &d) {
  return BrushFromColorObject(winrt::to_hstring(d.find("windowsbrush")->second.asString()));
}

xaml::Media::Brush BrushFromColorObject(const winrt::Microsoft::ReactNative::JSValue &v) {
  return BrushFromColorObject(winrt::to_hstring(v["windowsbrush"].AsString()));
}

winrt::Color ColorFromNumber(DWORD argb) noexcept {
  return xaml::FromArgb(GetAFromArgb(argb), GetRFromArgb(argb), GetGFromArgb(argb), GetBFromArgb(argb));
}

REACTWINDOWS_API_(winrt::Color) ColorFrom(const folly::dynamic &d) {
  if (!d.isNumber())
    return winrt::Colors::Transparent();
  return ColorFromNumber(static_cast<UINT>(d.asInt()));
}

winrt::Color ColorFrom(const winrt::Microsoft::ReactNative::JSValue &v) {
  if (v.Type() != winrt::Microsoft::ReactNative::JSValueType::Int64 &&
      v.Type() != winrt::Microsoft::ReactNative::JSValueType::Double)
    return winrt::Colors::Transparent();
  return ColorFromNumber(v.AsUInt32());
}

xaml::Media::SolidColorBrush SolidBrushFromColor(winrt::Color color) {
  thread_local static std::map<winrt::Color, winrt::weak_ref<xaml::Media::SolidColorBrush>, ColorComp>
      solidColorBrushCache;

  if (solidColorBrushCache.count(color) != 0) {
    if (auto brush = solidColorBrushCache[color].get()) {
      return brush;
    }
  }

  xaml::Media::SolidColorBrush brush(color);
  solidColorBrushCache[color] = winrt::make_weak(brush);
  return brush;
}

REACTWINDOWS_API_(xaml::Media::SolidColorBrush)
SolidColorBrushFrom(const winrt::Microsoft::ReactNative::JSValue &v) {
  if (v.Type() == winrt::Microsoft::ReactNative::JSValueType::Object) {
    return BrushFromColorObject(v).as<xaml::Media::SolidColorBrush>();
  }

  return SolidBrushFromColor(ColorFrom(v));
}

REACTWINDOWS_API_(xaml::Media::SolidColorBrush)
SolidColorBrushFrom(const folly::dynamic &d) {
  if (d.isObject()) {
    return BrushFromColorObject(d).as<xaml::Media::SolidColorBrush>();
  }

  return SolidBrushFromColor(ColorFrom(d));
}

REACTWINDOWS_API_(winrt::Brush) BrushFrom(const folly::dynamic &d) {
  if (d.isObject()) {
    return BrushFromColorObject(d);
  }

  return SolidColorBrushFrom(d);
}

REACTWINDOWS_API_(xaml::Media::Brush)
BrushFrom(const winrt::Microsoft::ReactNative::JSValue &v) {
  if (v.Type() == winrt::Microsoft::ReactNative::JSValueType::Object) {
    return BrushFromColorObject(v);
  }
  return SolidColorBrushFrom(v);
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

REACTWINDOWS_API_(winrt::TimeSpan) TimeSpanFromMs(double ms) {
  std::chrono::milliseconds dur((int64_t)ms);
  return dur;
}

// C# provides System.Uri.TryCreate, but no native equivalent seems to exist
winrt::Uri UriTryCreate(winrt::param::hstring const &uri) {
  try {
    return winrt::Uri(uri);
  } catch (...) {
    return winrt::Uri(nullptr);
  }
}

bool IsValidOptionalColorValue(const winrt::Microsoft::ReactNative::JSValue &v) {
  return v.Type() == winrt::Microsoft::ReactNative::JSValueType::Null || IsValidColorValue(v);
}

std::optional<winrt::Color> OptionalColorFrom(const winrt::Microsoft::ReactNative::JSValue &v) {
  return v.IsNull() ? std::optional<winrt::Color>{} : ColorFrom(v);
}

} // namespace Microsoft::ReactNative
