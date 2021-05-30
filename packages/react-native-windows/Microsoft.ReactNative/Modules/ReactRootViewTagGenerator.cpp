// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactRootViewTagGenerator.h"

namespace Microsoft::ReactNative {

// Keep in sync with ReactIOSTagHandles JS module - see that file for an explanation on why the
// increment here is 10.
const facebook::react::SurfaceId ROOT_VIEW_TAG_INCREMENT = 10;

std::atomic<facebook::react::SurfaceId> s_nextRootViewTag = 1;

facebook::react::SurfaceId getNextRootViewTag() noexcept {
  return s_nextRootViewTag += ROOT_VIEW_TAG_INCREMENT;
}

} // namespace Microsoft::ReactNative
