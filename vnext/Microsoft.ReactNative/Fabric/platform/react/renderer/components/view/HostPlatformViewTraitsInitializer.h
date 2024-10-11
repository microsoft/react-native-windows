// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/ShadowNodeTraits.h>

namespace facebook::react::HostPlatformViewTraitsInitializer {

inline bool formsStackingContext(ViewProps const &viewProps) {
  // onFocus/onBlur are often just used for listening to bubbled events.
  // Only Views which are marked as focusable can actually trigger the events, which will already avoid being collapsed.
  constexpr decltype(WindowsViewEvents::bits) focusEventsMask = {
      (1 << (int)WindowsViewEvents::Offset::Focus) & (1 << (int)WindowsViewEvents::Offset::Blur)};
  return (viewProps.windowsEvents.bits & focusEventsMask).any() || viewProps.tooltip;
}

inline bool formsView(ViewProps const &viewProps) {
  return viewProps.focusable;
}

inline ShadowNodeTraits::Trait extraTraits() {
  return ShadowNodeTraits::Trait::None;
}

} // namespace facebook::react::HostPlatformViewTraitsInitializer
