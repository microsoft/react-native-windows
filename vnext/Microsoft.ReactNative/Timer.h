// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Timer.g.h"

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct Timer : TimerT<Timer> {
  Timer() noexcept;

  static ITimer Create(const IReactPropertyBag &properties);
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct Timer : TimerT<Timer, implementation::Timer> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
