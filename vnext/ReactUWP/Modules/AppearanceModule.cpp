// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AppearanceModule.h"

#include <winrt/Windows.UI.ViewManagement.h>

using Application = winrt::Windows::UI::Xaml::Application;
using ApplicationTheme = winrt::Windows::UI::Xaml::ApplicationTheme;
using UISettings = winrt::Windows::UI::ViewManagement::UISettings;

using Method = facebook::xplat::module::CxxModule::Method;

namespace react::uwp {

AppearanceChangeListener::AppearanceChangeListener(std::weak_ptr<IReactInstance> &&reactInstance) noexcept
    : Mso::ActiveObject<>(Mso::DispatchQueue::MainUIQueue()), m_weakReactInstance(std::move(reactInstance)) {}

void AppearanceChangeListener::Initialize() noexcept {
  m_currentTheme = Application::Current().RequestedTheme();

  // UISettings will notify us on a background thread regardless of where we construct it or register for events. Let it
  // be constructed before init, but redirect callbacks to the UI thread where we can check app theme.
  m_uiSettings.ColorValuesChanged([weakThis{Mso::WeakPtr(this)}](const auto &, const auto &) noexcept {
    if (auto strongThis = weakThis.GetStrongPtr()) {
      strongThis->InvokeInQueueStrong([strongThis]() noexcept { strongThis->OnColorValuesChanged(); });
    }
  });

  m_inited.Set();
}

const char *AppearanceChangeListener::GetColorScheme() noexcept {
  // Wait until we have an initial valid value from the UI thread before returning anything
  m_inited.Wait();
  return ToString(m_currentTheme);
}

const char *AppearanceChangeListener::ToString(ApplicationTheme theme) noexcept {
  return theme == ApplicationTheme::Dark ? "dark" : "light";
}

void AppearanceChangeListener::OnColorValuesChanged() noexcept {
  auto newTheme = Application::Current().RequestedTheme();
  if (m_currentTheme != newTheme) {
    m_currentTheme = newTheme;

    if (auto reactInstance = m_weakReactInstance.lock()) {
      reactInstance->CallJsFunction(
          "RCTDeviceEventEmitter", "emit", folly::dynamic::array("appearanceChanged", ToString(m_currentTheme)));
    }
  }
}

AppearanceModule::AppearanceModule(std::weak_ptr<IReactInstance> &&reactInstance) noexcept
    : m_changeListener(Mso::Make<AppearanceChangeListener>(std::move(reactInstance))) {}

std::string AppearanceModule::getName() {
  return AppearanceModule::Name;
}

std::vector<Method> AppearanceModule::getMethods() {
  return {Method(
      "getColorScheme", [this](folly::dynamic /*args*/) { return m_changeListener->GetColorScheme(); }, SyncTag)};
}

} // namespace react::uwp
