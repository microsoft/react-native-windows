// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "QuirkSettings.h"
#include "QuirkSettings.g.cpp"

#include "QuirkSettings.h"
#include "ReactPropertyBag.h"

namespace winrt::Microsoft::ReactNative::implementation {

QuirkSettings::QuirkSettings() noexcept {}

/*static*/ void QuirkSettings::SetMatchAndroidAndIOSStretchBehavior(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  SetMatchAndroidAndIOSStretchBehavior(ReactPropertyBag(settings.Properties()), value);
}

winrt::Microsoft::ReactNative::ReactPropertyId<bool> MatchAndroidAndIOSStretchBehaviorProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{L"ReactNative.QuirkSettings",
                                                                     L"MatchAndroidAndIOSyStretchBehavior"};
  return propId;
}

/*static*/ void QuirkSettings::SetMatchAndroidAndIOSStretchBehavior(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    bool value) noexcept {
  properties.Set(MatchAndroidAndIOSStretchBehaviorProperty(), value);
}

/*static*/ bool QuirkSettings::GetMatchAndroidAndIOSStretchBehavior(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept {
  return properties.Get(MatchAndroidAndIOSStretchBehaviorProperty()).value_or(true);
}

} // namespace winrt::Microsoft::ReactNative::implementation
