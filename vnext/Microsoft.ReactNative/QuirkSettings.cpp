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

winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Microsoft::ReactNative::BackNavigationHandlerKind>
EnableBackHandlerKindProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Microsoft::ReactNative::BackNavigationHandlerKind> propId{
      L"ReactNative.QuirkSettings", L"EnableBackHandler"};

  return propId;
}
#pragma region IDL interface

/*static*/ void QuirkSettings::SetBackHandlerKind(
    winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
    winrt::Microsoft::ReactNative::BackNavigationHandlerKind kind) noexcept {
  ReactPropertyBag(settings.Properties()).Set(EnableBackHandlerKindProperty(), kind);
}

#pragma endregion IDL interface

/*static*/ winrt::Microsoft::ReactNative::BackNavigationHandlerKind QuirkSettings::GetBackHandlerKind(
    ReactPropertyBag properties) noexcept {
  return properties.Get(EnableBackHandlerKindProperty())
      .value_or(winrt::Microsoft::ReactNative::BackNavigationHandlerKind::JavaScript);
}

} // namespace winrt::Microsoft::ReactNative::implementation
