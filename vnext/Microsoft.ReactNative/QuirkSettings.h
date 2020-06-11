// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "QuirkSettings.g.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include "ReactPropertyBag.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct QuirkSettings : QuirkSettingsT<QuirkSettings> {
 public:
  QuirkSettings() noexcept;

  // public API - part of idl interface
  static void SetMatchAndroidAndIOSStretchBehavior(
      winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
      bool value) noexcept;

  // Internal use
  static void SetMatchAndroidAndIOSStretchBehavior(
      winrt::Microsoft::ReactNative::ReactPropertyBag properties,
      bool value) noexcept;
  static bool GetMatchAndroidAndIOSStretchBehavior(winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct QuirkSettings : QuirkSettingsT<QuirkSettings, implementation::QuirkSettings> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
