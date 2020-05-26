// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstanceSettings.h"
#if __has_include("ReactInstanceSettings.g.cpp")
#include "ReactInstanceSettings.g.cpp"
#endif

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

} // namespace winrt::Microsoft::ReactNative::implementation
