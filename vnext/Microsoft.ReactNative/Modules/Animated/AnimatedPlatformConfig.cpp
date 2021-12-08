// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AnimatedPlatformConfig.h"

namespace Microsoft::ReactNative {

// We could consider converting this value to a quirk setting in the future if
// we want to change the default behavior to use the frame rendering approach.
static constexpr auto DEFAULT_USE_COMPOSITION = true;

/*static*/ bool AnimatedPlatformConfig::ShouldUseComposition(
    const winrt::Microsoft::ReactNative::JSValueObject &config) {
  if (config.count(s_platformConfigName)) {
    const auto &platformConfig = config[s_platformConfigName].AsObject();
    if (platformConfig.count(s_useCompositionName)) {
      return platformConfig[s_useCompositionName].AsBoolean();
    }
  }
  return DEFAULT_USE_COMPOSITION;
}

} // namespace Microsoft::ReactNative
