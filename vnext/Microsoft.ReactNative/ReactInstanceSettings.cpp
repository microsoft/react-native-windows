// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstanceSettings.h"
#if __has_include("ReactInstanceSettings.g.cpp")
#include "ReactInstanceSettings.g.cpp"
#endif

#include "ReactHost/React.h"

namespace winrt::Microsoft::ReactNative::implementation {

ReactInstanceSettings::ReactInstanceSettings() noexcept {
  // Use current thread dispatcher as a default UI dispatcher.
  m_properties.Set(ReactDispatcherHelper::UIDispatcherProperty(), ReactDispatcherHelper::UIThreadDispatcher());
}

IReactDispatcher ReactInstanceSettings::UIDispatcher() noexcept {
  return m_properties.Get(ReactDispatcherHelper::UIDispatcherProperty()).try_as<IReactDispatcher>();
}

void ReactInstanceSettings::UIDispatcher(IReactDispatcher const &value) noexcept {
  m_properties.Set(ReactDispatcherHelper::UIDispatcherProperty(), value);
}

bool ReactInstanceSettings::UseDeveloperSupport() noexcept {
  return Mso::React::ReactOptions::UseDeveloperSupport(m_properties);
}

void ReactInstanceSettings::UseDeveloperSupport(bool value) noexcept {
  Mso::React::ReactOptions::SetUseDeveloperSupport(m_properties, value);
}

bool ReactInstanceSettings::UseFastRefresh() noexcept {
  return Mso::React::ReactOptions::UseFastRefresh(m_properties);
}

void ReactInstanceSettings::UseFastRefresh(bool value) noexcept {
  Mso::React::ReactOptions::SetUseFastRefresh(m_properties, value);
}

bool ReactInstanceSettings::UseLiveReload() noexcept {
  return Mso::React::ReactOptions::UseLiveReload(m_properties);
}

void ReactInstanceSettings::UseLiveReload(bool value) noexcept {
  Mso::React::ReactOptions::SetUseLiveReload(m_properties, value);
}

bool ReactInstanceSettings::UseWebDebugger() noexcept {
  return Mso::React::ReactOptions::UseWebDebugger(m_properties);
}

void ReactInstanceSettings::UseWebDebugger(bool value) noexcept {
  Mso::React::ReactOptions::SetUseWebDebugger(m_properties, value);
}

bool ReactInstanceSettings::UseDirectDebugger() noexcept {
  return Mso::React::ReactOptions::UseDirectDebugger(m_properties);
}

void ReactInstanceSettings::UseDirectDebugger(bool value) noexcept {
  Mso::React::ReactOptions::SetUseDirectDebugger(m_properties, value);
}

bool ReactInstanceSettings::DebuggerBreakOnNextLine() noexcept {
  return Mso::React::ReactOptions::DebuggerBreakOnNextLine(m_properties);
}

void ReactInstanceSettings::DebuggerBreakOnNextLine(bool value) noexcept {
  Mso::React::ReactOptions::SetDebuggerBreakOnNextLine(m_properties, value);
}

bool ReactInstanceSettings::EnableDeveloperMenu() noexcept {
  return UseDeveloperSupport();
}

void ReactInstanceSettings::EnableDeveloperMenu(bool value) noexcept {
  UseDeveloperSupport(value);
}

hstring ReactInstanceSettings::DebugHost() noexcept {
  std::wstring dhost(SourceBundleHost());
  dhost.append(L":");
  dhost.append(std::to_wstring(SourceBundlePort()));
  return hstring(dhost);
}

void ReactInstanceSettings::DebugHost(hstring const &value) noexcept {
  std::wstring dhost(value);
  auto colonPos = dhost.find(L':');
  if (colonPos != std::wstring::npos) {
    SourceBundleHost(hstring(dhost.substr(0, colonPos)));
    dhost.erase(0, colonPos + 1);
    SourceBundlePort(static_cast<uint16_t>(std::stoi(dhost)));
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation
