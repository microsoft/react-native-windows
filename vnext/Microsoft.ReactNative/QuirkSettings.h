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

  static winrt::Microsoft::ReactNative::BackNavigationHandlerKind GetBackHandlerKind(
      winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

  static bool GetEnableFabric(winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

#pragma region Public API - part of IDL interface

  static void SetBackHandlerKind(
      winrt::Microsoft::ReactNative::ReactInstanceSettings settings,
      winrt::Microsoft::ReactNative::BackNavigationHandlerKind kind) noexcept;

#pragma endregion Public API - part of IDL interface
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct QuirkSettings : QuirkSettingsT<QuirkSettings, implementation::QuirkSettings> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
