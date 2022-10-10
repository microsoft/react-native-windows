// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageRequest.h>

#include <ReactContext.h>

namespace Microsoft::ReactNative {

struct WindowsImageManager {
  WindowsImageManager();

  facebook::react::ImageRequest requestImage(
      const facebook::react::ImageSource &imageSource,
      facebook::react::SurfaceId surfaceId) const;
};

} // namespace Microsoft::ReactNative
