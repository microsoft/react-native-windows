// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "QuirkSettings.h"
#include "QuirkSettings.g.cpp"

#include "QuirkSettings.h"
#include "ReactPropertyBag.h"

namespace winrt::Microsoft::ReactNative::implementation {

QuirkSettings::QuirkSettings() noexcept {}

winrt::Microsoft::ReactNative::ReactPropertyId<bool> MatchAndroidAndIOSStretchBehaviorProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{
      L"ReactNative.QuirkSettings", L"MatchAndroidAndIOSyStretchBehavior"};
  return propId;
}

/*static*/ void QuirkSettings::SetMatchAndroidAndIOSStretchBehavior(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    bool value) noexcept {
  properties.Set(MatchAndroidAndIOSStretchBehaviorProperty(), value);
}

winrt::Microsoft::ReactNative::ReactPropertyId<bool> UseLegacyWebSocketModuleProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{
      L"ReactNative.QuirkSettings", L"UseLegacyWebSocketModule"};

  return propId;
}

winrt::Microsoft::ReactNative::ReactPropertyId<bool> AcceptSelfSignedCertsProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{
      L"ReactNative.QuirkSettings", L"Networking.AcceptSelfSigned"};

  return propId;
}

#pragma region IDL interface

/*static*/ void QuirkSettings::SetMatchAndroidAndIOSStretchBehavior(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  SetMatchAndroidAndIOSStretchBehavior(ReactPropertyBag(settings.Properties()), value);
}

/*static*/ void QuirkSettings::SetUseLegacyWebSocketModule(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  ReactPropertyBag(settings.Properties()).Set(UseLegacyWebSocketModuleProperty(), value);
}

/*static*/ void QuirkSettings::SetAcceptSelfSigned(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  ReactPropertyBag(settings.Properties()).Set(AcceptSelfSignedCertsProperty(), value);
}

#pragma endregion IDL interface

/*static*/ bool QuirkSettings::GetMatchAndroidAndIOSStretchBehavior(ReactPropertyBag properties) noexcept {
  return properties.Get(MatchAndroidAndIOSStretchBehaviorProperty()).value_or(true);
}

/*static*/ bool QuirkSettings::GetUseLegacyWebSocketModule(ReactPropertyBag properties) noexcept {
  return properties.Get(UseLegacyWebSocketModuleProperty()).value_or(false);
}

/*static*/ bool QuirkSettings::GetAcceptSelfSigned(ReactPropertyBag properties) noexcept {
  return properties.Get(AcceptSelfSignedCertsProperty()).value_or(false);
}

} // namespace winrt::Microsoft::ReactNative::implementation
