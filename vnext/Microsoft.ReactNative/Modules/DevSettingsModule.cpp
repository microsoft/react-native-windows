// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DevSettingsModule.h"
#include "IReactContext.h"

namespace Microsoft::ReactNative {

void DevSettings::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

struct ReloadFunctor
    : winrt::implements<ReloadFunctor, winrt::default_interface<winrt::Windows::Foundation::IInspectable>> {
  ReloadFunctor(Mso::VoidFunctor &&func) : m_func(std::move(func)) {}

  void operator()() {
    m_func();
  }

 private:
  Mso::VoidFunctor m_func;
};

/*static*/ void DevSettings::SetReload(Mso::React::ReactOptions const &options, Mso::VoidFunctor &&func) noexcept {
  options.Properties.Set(ReloadProperty().Handle(), winrt::make<ReloadFunctor>(std::move(func)));
}

/*static*/ winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Windows::Foundation::IInspectable>
DevSettings::ReloadProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Windows::Foundation::IInspectable> propId{
      L"ReactNative.DevSettings", L"Reload"};
  return propId;
}

void DevSettings::reload() noexcept {
  Reload(m_context.Properties());
}

/*static*/ void DevSettings::Reload(winrt::Microsoft::ReactNative::ReactPropertyBag const &properties) noexcept {
  (*winrt::get_self<ReloadFunctor>(properties.Get(
      winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Windows::Foundation::IInspectable>(ReloadProperty()))))();
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

/*static*/ void DevSettings::ToggleElementInspector(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  reactContext->CallJSFunction(
      "RCTDeviceEventEmitter", "emit", folly::dynamic::array("toggleElementInspector", nullptr));
}

void DevSettings::addMenuItem(std::string title) noexcept {
  assert(false);
}

void DevSettings::setIsShakeToShowDevMenuEnabled(bool enabled) noexcept {
  assert(false);
}

} // namespace Microsoft::ReactNative
