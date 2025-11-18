// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/ShadowNodeTraits.h>

namespace facebook::react::HostPlatformViewTraitsInitializer {

inline bool formsStackingContext(ViewProps const &viewProps) {
  return !viewProps.keyDownEvents.empty() || !viewProps.keyUpEvents.empty() || viewProps.tooltip;
}

inline bool formsView(ViewProps const &viewProps) {
  return viewProps.focusable;
}

inline ShadowNodeTraits::Trait extraTraits() {
  return ShadowNodeTraits::Trait::None;
}

inline bool isKeyboardFocusable(const ViewProps &viewProps) {
  return false;
}

} // namespace facebook::react::HostPlatformViewTraitsInitializer
