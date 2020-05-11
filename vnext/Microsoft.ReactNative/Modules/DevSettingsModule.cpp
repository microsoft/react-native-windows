// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DevSettingsModule.h"

namespace Microsoft::ReactNative {

void DevSettings::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext.Handle();
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
  options.Properties.Set(ReloadProperty(), winrt::make<ReloadFunctor>(std::move(func)));
}

/*static*/ winrt::Microsoft::ReactNative::IReactPropertyName DevSettings::ReloadProperty() noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"DevSettings"), L"Reload");
}

void DevSettings::reload() noexcept {
  (*winrt::get_self<ReloadFunctor>(m_context.Properties().Get(ReloadProperty())))();
}

void DevSettings::reloadWithReason(std::string /*reason*/) noexcept {
  reload();
}

void DevSettings::onFastRefresh() noexcept {
  // noop
}

void DevSettings::setHotLoadingEnabled(bool isHotLoadingEnabled) noexcept {
  assert(false);
}

void DevSettings::setIsDebuggingRemotely(bool isDebuggingRemotelyEnabled) noexcept {
  assert(false);
}

void DevSettings::setProfilingEnabled(bool isProfilingEnabled) noexcept {
  assert(false);
}

void DevSettings::toggleElementInspector() noexcept {
  assert(false);
}

void DevSettings::addMenuItem(std::string title) noexcept {
  assert(false);
}

void DevSettings::setIsShakeToShowDevMenuEnabled(bool enabled) noexcept {
  assert(false);
}

} // namespace Microsoft::ReactNative
