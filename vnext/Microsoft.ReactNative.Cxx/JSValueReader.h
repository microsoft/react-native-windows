// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
// IMPORTANT: Before updating this file
// please read react-native-windows repo:
// vnext/Microsoft.ReactNative.Cxx/README.md

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUEREADER
#define MICROSOFT_REACTNATIVE_JSVALUEREADER

#ifndef __APPLE__
#include <CppWinRTIncludes.h>
#include <UI.Xaml.Markup.h>
#include <chrono>
#endif
#include "JSValue.h"
#include "JSValueTreeReader.h"
#include "StructInfo.h"

#include "winrt/Microsoft.ReactNative.h"

#include <string>

namespace winrt::Microsoft::ReactNative {

// A value can be read from IJSValueReader in one of three ways:
// 1. Using a ReadValue standalone function with IJSValueReader& as a first argument.
// 2. Using a ReadValue standalone function with const JSValue& as a first argument.
// 3. We can auto-generate the read method for the type in some cases.

// A special type to prevent circular call between
// ReadValue(IJSValueReader&, T&) and ReadValue(JSValue&, T&)
template <class T>
struct TypeWrapper {
  T Value;
  operator T &() noexcept {
    return Value;
  }
};

//==============================================================================
// IJSValueReader extensions
//==============================================================================

// Forward declarations
template <class T>
T ReadValue(IJSValueReader const &reader) noexcept;
template <class T>
T ReadValue(const JSValue &jsValue) noexcept;

template <class T>
void SkipValue(IJSValueReader const &reader) noexcept;
template <class T>
void SkipValue(const JSValue &jsValue) noexcept;

template <class T, class TJSValueReader, std::enable_if_t<std::is_same_v<TJSValueReader, IJSValueReader>, int> = 1>
void ReadValue(TJSValueReader const &reader, /*out*/ T &value) noexcept;

template <class T, class TJSValue, std::enable_if_t<std::is_same_v<TJSValue, JSValue>, int> = 1>
void ReadValue(TJSValue const &jsValue, /*out*/ T &value) noexcept;

void ReadValue(IJSValueReader const &reader, /*out*/ std::string &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ std::wstring &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ winrt::hstring &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ bool &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ int8_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ int16_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ int32_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ int64_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ uint8_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ uint16_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ uint32_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ uint64_t &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ float &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ double &value) noexcept;
template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 1>
void ReadValue(IJSValueReader const &reader, /*out*/ T &value) noexcept;
template <class T>
void ReadValue(IJSValueReader const &reader, /*out*/ std::optional<T> &value) noexcept;
template <class T, class TCompare = std::less<>, class TAlloc = std::allocator<std::pair<const std::string, T>>>
void ReadValue(IJSValueReader const &reader, /*out*/ std::map<std::string, T, TCompare, TAlloc> &value) noexcept;
template <class T, class TCompare = std::less<>, class TAlloc = std::allocator<std::pair<const std::string, T>>>
void ReadValue(IJSValueReader const &reader, /*out*/ std::map<std::wstring, T, TCompare, TAlloc> &value) noexcept;
template <class T, class TAlloc = std::allocator<T>>
void ReadValue(IJSValueReader const &reader, /*out*/ std::vector<T, TAlloc> &value) noexcept;
template <class... Ts>
void ReadValue(IJSValueReader const &reader, /*out*/ std::tuple<Ts...> &value) noexcept;

void ReadValue(IJSValueReader const &reader, /*out*/ JSValue &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ JSValueObject &value) noexcept;
void ReadValue(IJSValueReader const &reader, /*out*/ JSValueArray &value) noexcept;

template <class T, std::enable_if_t<!std::is_void_v<decltype(GetStructInfo(static_cast<T *>(nullptr)))>, int> = 1>
void ReadValue(IJSValueReader const &reader, /*out*/ T &value) noexcept;

bool SkipArrayToEnd(IJSValueReader const &reader) noexcept;
template <class... TArgs>
void ReadArgs(IJSValueReader const &reader, /*out*/ TArgs &... args) noexcept;

//===========================================================================
// IJSValueReader extensions implementation
//===========================================================================

// This is a convenience method to call ReadValue for IJSValueReader.
template <class T>
inline T ReadValue(IJSValueReader const &reader) noexcept {
  T result;
  ReadValue(reader, /*out*/ result);
  return result;
}

// This is a convenience method to call ReadValue for JSValue.
template <class T>
inline T ReadValue(JSValue const &jsValue) noexcept {
  T result;
  ReadValue(jsValue, /*out*/ result);
  return result;
}

// Call ReadValue for IJSValueReader and ignore result.
template <class T>
inline void SkipValue(IJSValueReader const &reader) noexcept {
  T result;
  ReadValue(reader, /*out*/ result);
}

// Call ReadValue for JSValue and ignore result.
template <class T>
inline void SkipValue(const JSValue &jsValue) noexcept {
  T result;
  ReadValue(jsValue, /*out*/ result);
}

// Try to call ReadValue for JSValue unless it is already called us with TypeWrapper parameter.
template <class T, class TJSValueReader, std::enable_if_t<std::is_same_v<TJSValueReader, IJSValueReader>, int>>
inline void ReadValue(TJSValueReader const &reader, /*out*/ T &value) noexcept {
  TypeWrapper<JSValue> jsValue = {JSValue::ReadFrom(reader)};
  ReadValue(jsValue, /*out*/ value);
}

// Try to call ReadValue for IJSValueReader unless it is already called us with TypeWrapper parameter.
template <class T, class TJSValue, std::enable_if_t<std::is_same_v<TJSValue, JSValue>, int>>
inline void ReadValue(TJSValue const &jsValue, /*out*/ T &value) noexcept {
  TypeWrapper<IJSValueReader> reader = {MakeJSValueTreeReader(jsValue)};
  ReadValue(reader, /*out*/ value);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ std::string &value) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::String:
      value = to_string(reader.GetString());
      break;
    case JSValueType::Boolean:
      value = reader.GetBoolean() ? "true" : "false";
      break;
    case JSValueType::Int64:
      value = std::to_string(reader.GetInt64());
      break;
    case JSValueType::Double:
      value = std::to_string(reader.GetDouble());
      value.erase(value.find_last_not_of('0') + 1, std::string::npos);
      value.erase(value.find_last_not_of('.') + 1, std::string::npos);
      break;
    default:
      value = "";
      break;
  }
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ std::wstring &value) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::String:
      value = reader.GetString();
      break;
    case JSValueType::Boolean:
      value = reader.GetBoolean() ? L"true" : L"false";
      break;
    case JSValueType::Int64:
      value = std::to_wstring(reader.GetInt64());
      break;
    case JSValueType::Double:
      value = std::to_wstring(reader.GetDouble());
      value.erase(value.find_last_not_of('0') + 1, std::wstring::npos);
      value.erase(value.find_last_not_of('.') + 1, std::wstring::npos);
      break;
    default:
      value = L"";
      break;
  }
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ winrt::hstring &value) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::String:
      value = reader.GetString();
      break;
    case JSValueType::Boolean:
      value = reader.GetBoolean() ? L"true" : L"false";
      break;
    case JSValueType::Int64:
      value = to_hstring(reader.GetInt64());
      break;
    case JSValueType::Double: {
      std::wstring conversionValue = std::to_wstring(reader.GetDouble());
      conversionValue.erase(conversionValue.find_last_not_of('0') + 1, std::wstring::npos);
      conversionValue.erase(conversionValue.find_last_not_of('.') + 1, std::wstring::npos);
      value = conversionValue;
      break;
    }
    default:
      value = L"";
      break;
  }
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ bool &value) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::String:
      value = !to_string(reader.GetString()).empty();
      break;
    case JSValueType::Boolean:
      value = reader.GetBoolean();
      break;
    case JSValueType::Int64:
      value = reader.GetInt64() != 0;
      break;
    case JSValueType::Double:
      value = reader.GetDouble() != 0;
      break;
    default:
      value = false;
      break;
  }
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ int8_t &value) noexcept {
  int64_t val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<int8_t>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ int16_t &value) noexcept {
  int64_t val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<int16_t>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ int32_t &value) noexcept {
  int64_t val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<int32_t>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ int64_t &value) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::String: {
      hstring str = reader.GetString();
      wchar_t *end = nullptr;
      auto iValue = _wcstoi64(str.data(), &end, 10 /*base*/);
      if (end == str.data() + str.size()) {
        value = iValue;
      } else {
        value = 0;
      }
      break;
    }
    case JSValueType::Boolean:
      value = reader.GetBoolean() ? 1 : 0;
      break;
    case JSValueType::Int64:
      value = reader.GetInt64();
      break;
    case JSValueType::Double:
      value = static_cast<int64_t>(reader.GetDouble());
      break;
    default:
      value = 0;
      break;
  }
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ uint8_t &value) noexcept {
  int64_t val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<uint8_t>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ uint16_t &value) noexcept {
  int64_t val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<uint16_t>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ uint32_t &value) noexcept {
  int64_t val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<uint32_t>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ uint64_t &value) noexcept {
  int64_t val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<uint64_t>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ float &value) noexcept {
  double val;
  ReadValue(reader, /*out*/ val);
  value = static_cast<float>(val);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ double &value) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::String: {
      hstring str = reader.GetString();
      wchar_t *end = nullptr;
      auto dvalue = wcstod(str.data(), &end);
      if (end == str.data() + str.size()) {
        value = dvalue;
      } else {
        value = 0;
      }
      break;
    }
    case JSValueType::Boolean:
      value = reader.GetBoolean() ? 1 : 0;
      break;
    case JSValueType::Int64:
      value = static_cast<double>(reader.GetInt64());
      break;
    case JSValueType::Double:
      value = reader.GetDouble();
      break;
    default:
      value = 0;
      break;
  }
}

template <class T, std::enable_if_t<std::is_enum_v<T>, int>>
inline void ReadValue(IJSValueReader const &reader, /*out*/ T &value) noexcept {
  int32_t intValue;
  ReadValue(reader, intValue);
  value = static_cast<T>(intValue);
}

template <class T>
inline void ReadValue(IJSValueReader const &reader, /*out*/ std::optional<T> &value) noexcept {
  if (reader.ValueType() != JSValueType::Null) {
    value = ReadValue<T>(reader);
  } else {
    value = std::nullopt;
  }
}

// Note that we use std::less<> as a default comparer instead of standard std::less<Key>.
// This is to enable use of string-like objects such as std::string_view as key to retrieve values.
// While std::less<> is better, the standard cannot have a breaking change to switch to it.
template <class T, class TCompare, class TAlloc>
inline void ReadValue(
    IJSValueReader const &reader,
    /*out*/ std::map<std::string, T, TCompare, TAlloc> &value) noexcept {
  if (reader.ValueType() == JSValueType::Object) {
    hstring propertyName;
    while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
      value.emplace(to_string(propertyName), ReadValue<T>(reader));
    }
  }
}

template <class T, class TCompare, class TAlloc>
inline void ReadValue(
    IJSValueReader const &reader,
    /*out*/ std::map<std::wstring, T, TCompare, TAlloc> &value) noexcept {
  if (reader.ValueType() == JSValueType::Object) {
    hstring propertyName;
    while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
      value.emplace(propertyName, ReadValue<T>(reader));
    }
  }
}

template <class T, class TAlloc>
inline void ReadValue(IJSValueReader const &reader, /*out*/ std::vector<T, TAlloc> &value) noexcept {
  if (reader.ValueType() == JSValueType::Array) {
    while (reader.GetNextArrayItem()) {
      value.push_back(ReadValue<T>(reader));
    }
  }
}

template <class T, size_t... I>
inline void ReadTuple(IJSValueReader const &reader, /*out*/ T &tuple, std::index_sequence<I...>) noexcept {
  ReadArgs(reader, std::get<I>(tuple)...);
}

template <class... Ts>
inline void ReadValue(IJSValueReader const &reader, /*out*/ std::tuple<Ts...> &value) noexcept {
  ReadTuple(reader, value, std::make_index_sequence<sizeof...(Ts)>{});
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ JSValue &value) noexcept {
  value = JSValue::ReadFrom(reader);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ JSValueObject &value) noexcept {
  value = JSValueObject::ReadFrom(reader);
}

inline void ReadValue(IJSValueReader const &reader, /*out*/ JSValueArray &value) noexcept {
  value = JSValueArray::ReadFrom(reader);
}

#ifndef __APPLE__
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
#endif

template <class T, std::enable_if_t<!std::is_void_v<decltype(GetStructInfo(static_cast<T *>(nullptr)))>, int>>
inline void ReadValue(IJSValueReader const &reader, /*out*/ T &value) noexcept {
  if (reader.ValueType() == JSValueType::Object) {
    const auto &fieldMap = StructInfo<T>::FieldMap;
    hstring propertyName;
    while (reader.GetNextObjectProperty(/*out*/ propertyName)) {
      auto it = fieldMap.find(std::wstring_view(propertyName));
      if (it != fieldMap.end()) {
        it->second.ReadField(reader, &value);
      } else {
        SkipValue<JSValue>(reader); // Skip this property
      }
    }
  }
}

// It helps to read arguments from an array if there are more items than expected.
inline bool SkipArrayToEnd(IJSValueReader const &reader) noexcept {
  while (reader.GetNextArrayItem()) {
    SkipValue<JSValue>(reader); // Read and ignore the value
  }

  return true;
}

template <class... TArgs>
inline void ReadArgs(IJSValueReader const &reader, /*out*/ TArgs &... args) noexcept {
  // Read as many arguments as we can or return default values.
  bool success = reader.ValueType() == JSValueType::Array;
  ((success = success && reader.GetNextArrayItem(), args = success ? ReadValue<TArgs>(reader) : TArgs{}), ...);
  success = success && SkipArrayToEnd(reader);
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSVALUEREADER
