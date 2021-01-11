// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#pragma warning(push)
#pragma warning(disable : 4244)
#include <react/renderer/core/ReactPrimitives.h>
#pragma warning(pop)

namespace Microsoft::ReactNative {

facebook::react::SurfaceId getNextRootViewTag() noexcept;

}
