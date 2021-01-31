// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <CppWinRTIncludes.h>
#include <UI.Xaml.Markup.h>
#include <chrono>

#include "JSValue.h"
#include "JSValueTreeReader.h"
#include "StructInfo.h"

#include "winrt/Microsoft.ReactNative.h"

#include <string>

namespace winrt::Microsoft::ReactNative {
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

inline ui::Color ColorFromArgb(const JSValue &value) {
  auto argb = static_cast<DWORD>(value.AsInt64());
  return winrt::ColorHelper::FromArgb(GetAFromArgb(argb), GetRFromArgb(argb), GetGFromArgb(argb), GetBFromArgb(argb));
}

inline ui::Color UndefinedColor() noexcept {
  static auto c = ColorHelper::FromArgb(1, 0, 0, 0);
  return c;
}

inline xaml::Media::SolidColorBrush UndefinedSolidColorBrush() noexcept {
  static auto b = xaml::Media::SolidColorBrush(UndefinedColor());
  return b;
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ ui::Color &value) {
  value = UndefinedColor();
  if (reader.ValueType() == JSValueType::Int64) {
    auto argb = static_cast<DWORD>(reader.GetInt64());
    value =
        winrt::ColorHelper::FromArgb(GetAFromArgb(argb), GetRFromArgb(argb), GetGFromArgb(argb), GetBFromArgb(argb));
  }
}

struct ColorComp {
  bool operator()(const ui::Color &lhs, const ui::Color &rhs) const {
    return (
        lhs.A < rhs.A ||
        lhs.A == rhs.A && (lhs.R < rhs.R || lhs.R == rhs.R && (lhs.G < rhs.G || lhs.G == rhs.G && lhs.B < rhs.B)));
  }
};

inline void ReadValue(IJSValueReader const &reader, /*out*/ xaml::Media::SolidColorBrush &value) {
  value = UndefinedSolidColorBrush();
  if (reader.ValueType() == JSValueType::Object) {
    winrt::hstring propertyName;
    while (reader.GetNextObjectProperty(propertyName)) {
      constexpr std::wstring_view windowsbrush = L"windowsbrush";
      if (propertyName != windowsbrush && reader.ValueType() == JSValueType::String) {
        auto resourceName = reader.GetString();

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
            value = brush;
            return;
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

          auto dictionary{winrt::unbox_value<xaml::ResourceDictionary>(xaml::Markup::XamlReader::Load(xamlString))};

          auto brush{
              winrt::unbox_value<xaml::Media::SolidColorBrush>(dictionary.Lookup(winrt::box_value(resourceName)))};

          accentColorMap[resourceName] = winrt::make_weak(brush);

          value = brush;
          return;
        }

        winrt::IInspectable resource{xaml::Application::Current().Resources().Lookup(winrt::box_value(resourceName))};

        value = winrt::unbox_value<xaml::Media::SolidColorBrush>(resource);
        return;
      }
    }
    return;
  }

  ui::Color color;
  ReadValue(reader, color);

  thread_local static std::map<ui::Color, winrt::weak_ref<xaml::Media::SolidColorBrush>, ColorComp>
      solidColorBrushCache;

  if (solidColorBrushCache.count(color) != 0) {
    if (auto brush = solidColorBrushCache[color].get()) {
      value = brush;
      return;
    }
  }

  value = xaml::Media::SolidColorBrush(color);
  solidColorBrushCache[color] = winrt::make_weak(value);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ xaml::VerticalAlignment &value) {
  value = xaml::VerticalAlignment::Stretch;
  if (reader.ValueType() == JSValueType::String) {
    const auto valueString = reader.GetString();
    if (valueString == L"bottom")
      value = xaml::VerticalAlignment::Bottom;
    else if (valueString == L"center")
      value = xaml::VerticalAlignment::Center;
    else if (valueString == L"stretch")
      value = xaml::VerticalAlignment::Stretch;
    else if (valueString == L"top")
      value = xaml::VerticalAlignment::Top;
    else {
      // ASSERT: Invalid value for VerticalAlignment. Shouldn't get this far.
      assert(false);
    }
  } else {
    assert(false);
  }
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ xaml::HorizontalAlignment &value) {
  value = xaml::HorizontalAlignment::Stretch;
  if (reader.ValueType() == JSValueType::String) {
    const auto valueString = reader.GetString();
    if (valueString == L"center")
      value = xaml::HorizontalAlignment::Center;
    else if (valueString == L"left")
      value = xaml::HorizontalAlignment::Left;
    else if (valueString == L"right")
      value = xaml::HorizontalAlignment::Right;
    else if (valueString == L"stretch")
      value = xaml::HorizontalAlignment::Stretch;
    else
      assert(false);
    // ASSERT: Invalid value for VerticalAlignment. Shouldn't get this far.
  } else
    assert(false);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ winrt::Windows::Foundation::DateTime &value) {
  int64_t elapsed = 0;
  if (reader.ValueType() == JSValueType::Int64) {
    elapsed = reader.GetInt64();
  }
  // A JavaScript date is specified as the number of milliseconds that have elapsed since midnight on January 1, 1970,
  // UTC.
  const auto ms = std::chrono::milliseconds(elapsed);
  value = winrt::Windows::Foundation::DateTime(ms);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ winrt::Windows::Foundation::TimeSpan &value) {
  int64_t elapsed = 0;
  if (reader.ValueType() == JSValueType::Int64) {
    elapsed = reader.GetInt64();
  }
  // A JavaScript date is specified as the number of milliseconds that have elapsed since midnight on January 1, 1970,
  // UTC.
  const auto ms = std::chrono::milliseconds(elapsed);
  value = winrt::Windows::Foundation::TimeSpan(ms);
}

} // namespace winrt::Microsoft::ReactNative
