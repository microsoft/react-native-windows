// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DevSettingsModule.h"
#include "IReactContext.h"

namespace Microsoft::ReactNative {

React::ReactPropertyId<React::ReactNonAbiValue<Mso::VoidFunctor>> ReloadProperty() noexcept {
  static React::ReactPropertyId<React::ReactNonAbiValue<Mso::VoidFunctor>> propId{L"ReactNative.DevSettings",
                                                                                  L"Reload"};
  return propId;
}

void DevSettings::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

/*static*/ void DevSettings::SetReload(Mso::React::ReactOptions const &options, Mso::VoidFunctor &&func) noexcept {
  React::ReactPropertyBag(options.Properties).Set(ReloadProperty(), std::move(func));
}

void DevSettings::reload() noexcept {
  Reload(m_context.Properties());
}

/*static*/ void DevSettings::Reload(winrt::Microsoft::ReactNative::ReactPropertyBag const &properties) noexcept {
  properties.Get(ReloadProperty())();
}

void DevSettings::reloadWithReason(std::string /*reason*/) noexcept {
  reload();
}

void DevSettings::onFastRefresh() noexcept {
  // noop
}

void DevSettings::setHotLoadingEnabled(bool isHotLoadingEnabled) noexcept {
  Mso::React::ReactOptions::SetUseFastRefresh(m_context.Properties().Handle(), isHotLoadingEnabled);
}

void DevSettings::setIsDebuggingRemotely(bool isDebuggingRemotelyEnabled) noexcept {
  Mso::React::ReactOptions::SetUseWebDebugger(m_context.Properties().Handle(), isDebuggingRemotelyEnabled);
}

void DevSettings::setProfilingEnabled(bool isProfilingEnabled) noexcept {
  assert(false);
}

void DevSettings::toggleElementInspector() noexcept {
  auto contextSelf = winrt::get_self<React::implementation::ReactContext>(m_context.Handle());
  ToggleElementInspector(contextSelf->GetInner());
}

/*static*/ void DevSettings::ToggleElementInspector(Mso::React::IReactContext &reactContext) noexcept {
  reactContext.CallJSFunction(
      "RCTDeviceEventEmitter", "emit", folly::dynamic::array("toggleElementInspector", nullptr));
}

void DevSettings::addMenuItem(std::string title) noexcept {
  assert(false);
}

void DevSettings::setIsShakeToShowDevMenuEnabled(bool enabled) noexcept {
  assert(false);
}

/*static*/ void DevSettings::addListener(std::string eventName) noexcept {
  // noop
}

/*static*/ void DevSettings::removeListeners(double count) noexcept {
  // noop
}

} // namespace Microsoft::ReactNative
