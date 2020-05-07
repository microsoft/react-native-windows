// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstanceSettings.h"
#if __has_include("ReactInstanceSettings.g.cpp")
#include "ReactInstanceSettings.g.cpp"
#endif

namespace winrt::Microsoft::ReactNative::implementation {

/*static*/ ReactPropertyBag ReactInstanceSettings::GetGlobalProperties() noexcept {
  static ReactPropertyBag s_globalProperties;
  return s_globalProperties;
}

} // namespace winrt::Microsoft::ReactNative::implementation
