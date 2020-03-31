// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUEXAML
#define MICROSOFT_REACTNATIVE_JSVALUEXAML

#include "JSValue.h"

namespace winrt::Microsoft::ReactNative {

#ifndef CXXUNITTESTS

inline void ReadValue(JSValue const &jsValue, Windows::UI::Xaml::Media::Brush &value) noexcept {
  value = XamlHelper::BrushFrom([&jsValue](IJSValueWriter const &writer) noexcept { jsValue.WriteTo(writer); });
}

inline void ReadValue(JSValue const &jsValue, Windows::UI::Color &value) noexcept {
  value = XamlHelper::ColorFrom([&jsValue](IJSValueWriter const &writer) noexcept { jsValue.WriteTo(writer); });
}

#endif

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSVALUEXAML
