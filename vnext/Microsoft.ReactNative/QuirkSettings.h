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

  // Internal use
  static void SetMatchAndroidAndIOSStretchBehavior(
      winrt::Microsoft::ReactNative::ReactPropertyBag properties,
      bool value) noexcept;
  static bool GetMatchAndroidAndIOSStretchBehavior(winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

  static void SetUseWebFlexBasisBehavior(
      winrt::Microsoft::ReactNative::ReactPropertyBag properties,
      bool value) noexcept;
  static bool GetUseWebFlexBasisBehavior(winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

  static bool GetAcceptSelfSigned(winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;
  static winrt::Microsoft::ReactNative::BackNavigationHandlerKind GetBackHandlerKind(
      winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

  static bool GetEnableFabric(winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

  static void SetMapWindowDeactivatedToAppStateInactive(
      winrt::Microsoft::ReactNative::ReactPropertyBag properties,
      bool value) noexcept;
  static bool GetMapWindowDeactivatedToAppStateInactive(
      winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

#pragma region Public API - part of IDL interface
  static void SetMatchAndroidAndIOSStretchBehavior(
      winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
      bool value) noexcept;

  static void SetUseWebFlexBasisBehavior(
      winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
      bool value) noexcept;

  static void SetAcceptSelfSigned(winrt::Microsoft::ReactNative::ReactInstanceSettings settings, bool value) noexcept;
  static void SetBackHandlerKind(
      winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
      winrt::Microsoft::ReactNative::BackNavigationHandlerKind kind) noexcept;

  static void SetMapWindowDeactivatedToAppStateInactive(
      winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
      bool value) noexcept;
#pragma endregion Public API - part of IDL interface
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct QuirkSettings : QuirkSettingsT<QuirkSettings, implementation::QuirkSettings> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
