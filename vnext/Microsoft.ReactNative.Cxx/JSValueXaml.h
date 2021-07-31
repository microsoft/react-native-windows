// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUEXAML
#define MICROSOFT_REACTNATIVE_JSVALUEXAML
#include "CppWinRTIncludes.h"
#include "JSValue.h"

namespace winrt::Microsoft::ReactNative {

#ifndef CXXUNITTESTS

inline void ReadValue(JSValue const &jsValue, xaml::Media::Brush &value) noexcept {
  value = XamlHelper::BrushFrom([&jsValue](IJSValueWriter const &writer) noexcept { jsValue.WriteTo(writer); });
}

inline void ReadValue(JSValue const &jsValue, Windows::UI::Color &value) noexcept {
  value = XamlHelper::ColorFrom([&jsValue](IJSValueWriter const &writer) noexcept { jsValue.WriteTo(writer); });
}

inline void ReadValue(JSValue const &jsValue, xaml::Thickness &value) noexcept {
  if (auto array = jsValue.TryGetArray()) {
    if (array->size() == 4) {
      value = ThicknessHelper::FromLengths(
          (*array)[0].AsDouble(), (*array)[1].AsDouble(), (*array)[2].AsDouble(), (*array)[3].AsDouble());
      return;
    }
  } else if (auto number = jsValue.TryGetDouble()) {
    value = ThicknessHelper::FromUniformLength(*number);
  } else if (auto numberInt = jsValue.TryGetInt64()) {
    const auto valueDbl = static_cast<double>(*numberInt);
    value = ThicknessHelper::FromUniformLength(valueDbl);
  } else {
    const auto &obj = jsValue.AsObject();
    value = ThicknessHelper::FromLengths(
        obj["left"].AsDouble(), obj["top"].AsDouble(), obj["right"].AsDouble(), obj["bottom"].AsDouble());
    return;
  }
}

inline void ReadValue(JSValue const &jsValue, xaml::CornerRadius &value) noexcept {
  if (auto array = jsValue.TryGetArray()) {
    if (array->size() == 4) {
      value = CornerRadiusHelper::FromRadii(
          (*array)[0].AsDouble(), (*array)[1].AsDouble(), (*array)[2].AsDouble(), (*array)[3].AsDouble());
      return;
    }
  } else if (auto number = jsValue.TryGetDouble()) {
    value = CornerRadiusHelper::FromUniformRadius(*number);
  } else if (auto numberInt = jsValue.TryGetInt64()) {
    const auto valueDbl = static_cast<double>(*numberInt);
    value = CornerRadiusHelper::FromUniformRadius(valueDbl);
  } else {
    const auto &obj = jsValue.AsObject();
    value = CornerRadiusHelper::FromRadii(
        obj["topLeft"].AsDouble(),
        obj["topRight"].AsDouble(),
        obj["bottomRight"].AsDouble(),
        obj["bottomLeft"].AsDouble());
    return;
  }
}

inline void ReadValue(JSValue const &jsValue, winrt::Windows::Foundation::Uri &value) noexcept {
  value = Uri{winrt::to_hstring(jsValue.AsString())};
}

#endif

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSVALUEXAML
