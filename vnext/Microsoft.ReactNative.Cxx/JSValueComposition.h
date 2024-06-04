// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUECOMPOSITION
#define MICROSOFT_REACTNATIVE_JSVALUECOMPOSITION
#include <winrt/Microsoft.ReactNative.h>
#include "JSValue.h"

namespace winrt::Microsoft::ReactNative {

inline void ReadValue(IJSValueReader const &reader, Color &value) noexcept {
  value = winrt::Microsoft::ReactNative::Color::ReadValue(reader);
}

inline void WriteValue(IJSValueWriter const &writer, const Color &value) noexcept {
  winrt::Microsoft::ReactNative::Color::WriteValue(writer, value);
}

inline void ReadValue(IJSValueReader const &reader, ImageSource &value) noexcept {
  value = winrt::Microsoft::ReactNative::ImageSource::ReadValue(reader);
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSVALUECOMPOSITION
