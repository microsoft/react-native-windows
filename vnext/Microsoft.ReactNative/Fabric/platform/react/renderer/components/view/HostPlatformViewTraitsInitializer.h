// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/ShadowNodeTraits.h>

namespace facebook::react::HostPlatformViewTraitsInitializer {

inline bool formsStackingContext(ViewProps const &viewProps) {
  return viewProps.windowsEvents.bits.any();
}

inline bool formsView(ViewProps const &viewProps) {
  return viewProps.focusable;
}

inline ShadowNodeTraits::Trait extraTraits() {
  return ShadowNodeTraits::Trait::None;
}

} // namespace facebook::react::HostPlatformViewTraitsInitializer
