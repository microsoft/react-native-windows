// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "QuirkSettings.h"
#include "QuirkSettings.g.cpp"

#include "QuirkSettings.h"
#include "React.h"
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

winrt::Microsoft::ReactNative::ReactPropertyId<bool> UseWebFlexBasisBehaviorProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{
      L"ReactNative.QuirkSettings", L"UseWebFlexBasisBehavior"};
  return propId;
}

/*static*/ void QuirkSettings::SetUseWebFlexBasisBehavior(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    bool value) noexcept {
  properties.Set(UseWebFlexBasisBehaviorProperty(), value);
}

winrt::Microsoft::ReactNative::ReactPropertyId<bool> AcceptSelfSignedCertsProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{
      L"ReactNative.QuirkSettings", L"Networking.AcceptSelfSigned"};

  return propId;
}

winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Microsoft::ReactNative::BackNavigationHandlerKind>
EnableBackHandlerKindProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Microsoft::ReactNative::BackNavigationHandlerKind> propId{
      L"ReactNative.QuirkSettings", L"EnableBackHandler"};

  return propId;
}

winrt::Microsoft::ReactNative::ReactPropertyId<bool> MapWindowDeactivatedToAppStateInactiveProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{
      L"ReactNative.QuirkSettings", L"MapWindowDeactivatedToAppStateInactiveProperty"};
  return propId;
}

/*static*/ void QuirkSettings::SetMapWindowDeactivatedToAppStateInactive(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    bool value) noexcept {
  properties.Set(MapWindowDeactivatedToAppStateInactiveProperty(), value);
}

#pragma region IDL interface

/*static*/ void QuirkSettings::SetMatchAndroidAndIOSStretchBehavior(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  SetMatchAndroidAndIOSStretchBehavior(ReactPropertyBag(settings.Properties()), value);
}

/*static*/ void QuirkSettings::SetUseWebFlexBasisBehavior(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  SetUseWebFlexBasisBehavior(ReactPropertyBag(settings.Properties()), value);
}

/*static*/ void QuirkSettings::SetAcceptSelfSigned(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  ReactPropertyBag(settings.Properties()).Set(AcceptSelfSignedCertsProperty(), value);
}

/*static*/ void QuirkSettings::SetBackHandlerKind(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    winrt::Microsoft::ReactNative::BackNavigationHandlerKind kind) noexcept {
  ReactPropertyBag(settings.Properties()).Set(EnableBackHandlerKindProperty(), kind);
}

/*static*/ void QuirkSettings::SetMapWindowDeactivatedToAppStateInactive(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    bool value) noexcept {
  SetMapWindowDeactivatedToAppStateInactive(ReactPropertyBag(settings.Properties()), value);
}

#pragma endregion IDL interface

/*static*/ bool QuirkSettings::GetMatchAndroidAndIOSStretchBehavior(ReactPropertyBag properties) noexcept {
  return properties.Get(MatchAndroidAndIOSStretchBehaviorProperty()).value_or(true);
}

/*static*/ bool QuirkSettings::GetUseWebFlexBasisBehavior(ReactPropertyBag properties) noexcept {
  return properties.Get(UseWebFlexBasisBehaviorProperty()).value_or(false);
}

/*static*/ bool QuirkSettings::GetAcceptSelfSigned(ReactPropertyBag properties) noexcept {
  return properties.Get(AcceptSelfSignedCertsProperty()).value_or(false);
}

/*static*/ winrt::Microsoft::ReactNative::BackNavigationHandlerKind QuirkSettings::GetBackHandlerKind(
    ReactPropertyBag properties) noexcept {
  return properties.Get(EnableBackHandlerKindProperty())
      .value_or(winrt::Microsoft::ReactNative::BackNavigationHandlerKind::JavaScript);
}

/*static*/ bool QuirkSettings::GetMapWindowDeactivatedToAppStateInactive(ReactPropertyBag properties) noexcept {
  return properties.Get(MapWindowDeactivatedToAppStateInactiveProperty()).value_or(false);
}

} // namespace winrt::Microsoft::ReactNative::implementation
