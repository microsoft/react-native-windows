// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUEREADER
#define MICROSOFT_REACTNATIVE_JSVALUEREADER

#include "winrt/Microsoft.ReactNative.Bridge.h"
#include "winrt/Microsoft.ReactNative.h"

//==============================================================================
// IJSValueReader helpers
//==============================================================================
namespace Microsoft::ReactNative {

template <class T>
inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    T &value) noexcept {
  static_assert(
      sizeof(std::decay_t<T>) == 0, "Implement ReadValue for the T type");
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    bool &value) noexcept {
  return reader.TryGetBoolen(value);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int8_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<int8_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int16_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<int16_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int32_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<int32_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    int64_t &value) noexcept {
  return reader.TryGetInt64(value);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint8_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint8_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint16_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint16_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint32_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint32_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    uint64_t &value) noexcept {
  int64_t result{};
  return reader.TryGetInt64(result) &&
      (value = static_cast<uint64_t>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    double &value) noexcept {
  return reader.TryGetDouble(value);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    float &value) noexcept {
  double result{};
  return reader.TryGetDouble(result) &&
      (value = static_cast<float>(result), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    std::string &value) noexcept {
  winrt::hstring str;
  return reader.TryGetString(str) && (value = winrt::to_string(str), true);
}

inline bool ReadValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    std::wstring &value) noexcept {
  winrt::hstring str;
  return reader.TryGetString(str) &&
      (value = std::wstring{str.data(), str.size()}, true);
}

template <class... TArgs>
inline void ReadArgs(
    winrt::Microsoft::ReactNative::Bridge::IJSValueReader const &reader,
    TArgs &... args) noexcept {
  if constexpr (sizeof...(args) > 0) {
    using winrt::Microsoft::ReactNative::Bridge::JSValueReaderState;
    JSValueReaderState state = reader.ReadNext();
    if (state == JSValueReaderState::ArrayBegin) {
      // To read variadic template arguments in natural order we must use them
      // in an initializer list.
      [[maybe_unused]] int dummy[] = {
          (state = reader.ReadNext(),
           state != JSValueReaderState::ArrayEnd && ReadValue(reader, args),
           0)...};
    }
  }
}

} // namespace Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSVALUEREADER
