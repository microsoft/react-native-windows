// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <JSValue.h>

namespace Microsoft::ReactNative {
class AnimatedPlatformConfig {
 public:
  static bool ShouldUseComposition(const winrt::Microsoft::ReactNative::JSValueObject &config);

 private:
  static constexpr std::string_view s_platformConfigName{"platformConfig"};
  static constexpr std::string_view s_useCompositionName{"useComposition"};
};
} // namespace Microsoft::ReactNative
