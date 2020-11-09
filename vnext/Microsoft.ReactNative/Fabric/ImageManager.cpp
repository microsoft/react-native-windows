// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#include <react/renderer/imagemanager/ImageManager.h>
#pragma warning(pop)

namespace facebook {
namespace react {

ImageManager::ImageManager(ContextContainer::Shared const &contextContainer) {
  // Silence unused-private-field warning.
  (void)self_;
  // Not implemented.
}

ImageManager::~ImageManager() {
  // Not implemented.
}

ImageRequest ImageManager::requestImage(
    const ImageSource &imageSource,
    SurfaceId surfaceId) const {
  // Not implemented.
  return ImageRequest(imageSource, nullptr);
}

} // namespace react
} // namespace facebook
