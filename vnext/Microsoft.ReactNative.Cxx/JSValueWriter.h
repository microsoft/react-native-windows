// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUEWRITER
#define MICROSOFT_REACTNATIVE_JSVALUEWRITER

#include "winrt/Microsoft.ReactNative.Bridge.h"
#include "winrt/Microsoft.ReactNative.h"


//==============================================================================
// IJSValueWriter helpers
//==============================================================================
namespace Microsoft::ReactNative {

template <class T>
inline bool WriteProperty(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const std::string &name,
    T value) noexcept {
  return writer.WritePropertyName(winrt::to_hstring(name)) &&
      WriteValue(writer, value);
}

template <class T>
inline bool WriteProperty(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const char *name,
    T value) noexcept {
  return writer.WritePropertyName(winrt::to_hstring(name)) &&
      WriteValue(writer, value);
}

template <class T>
inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    T value) noexcept {
  static_assert(
      sizeof(std::decay_t<T>) == 0, "Implement WriteValue for the T type");
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    bool value) noexcept {
  return writer.WriteBoolean(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int8_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int16_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int32_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    int64_t value) noexcept {
  return writer.WriteInt64(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint8_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint16_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint32_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    uint64_t value) noexcept {
  return writer.WriteInt64(static_cast<int64_t>(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    double value) noexcept {
  return writer.WriteDouble(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    float value) noexcept {
  return writer.WriteDouble(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const std::string &value) noexcept {
  return writer.WriteString(winrt::to_hstring(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const char *value) noexcept {
  return writer.WriteString(winrt::to_hstring(value));
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const std::wstring &value) noexcept {
  return writer.WriteString(value);
}

inline bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const wchar_t *value) noexcept {
  return writer.WriteString(value);
}

template <class... TArgs>
inline void WriteArgs(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const TArgs &... args) noexcept {
  writer.WriteArrayBegin();

  if constexpr (sizeof...(args) > 0) {
    // To write variadic template arguments in natural order we must use them in
    // an initializer list.
    [[maybe_unused]] int dummy[] = {(WriteValue(writer, args), 0)...};
  }

  writer.WriteArrayEnd();
}

} // namespace Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSVALUEWRITER
