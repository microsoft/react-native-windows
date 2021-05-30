// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageManager.h>

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

ImageRequest ImageManager::requestImage(const ImageSource &imageSource, SurfaceId surfaceId) const {
  // Not implemented.
  return ImageRequest(imageSource, nullptr);
}

} // namespace react
} // namespace facebook
