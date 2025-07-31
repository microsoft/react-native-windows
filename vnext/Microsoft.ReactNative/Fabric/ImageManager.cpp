// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageManager.h>
#include "WindowsImageManager.h"

namespace facebook {
namespace react {

ImageManager::ImageManager(std::shared_ptr<const ContextContainer> const &contextContainer) {
  auto reactContext = *contextContainer->find<winrt::Microsoft::ReactNative::ReactContext>("MSRN.ReactContext");
  self_ = new Microsoft::ReactNative::WindowsImageManager(reactContext);
}

ImageManager::~ImageManager() {
  delete (Microsoft::ReactNative::WindowsImageManager *)self_;
}

ImageRequest ImageManager::requestImage(const ImageSource &imageSource, SurfaceId surfaceId) const {
  return ((Microsoft::ReactNative::WindowsImageManager *)self_)->requestImage(imageSource, surfaceId);
}

ImageRequest ImageManager::requestImage(
    const ImageSource &imageSource,
    SurfaceId surfaceId,
    const ImageRequestParams &imageRequestParams,
    Tag tag) const {
  return ((Microsoft::ReactNative::WindowsImageManager *)self_)
      ->requestImage(imageSource, surfaceId, imageRequestParams, tag);
}

} // namespace react
} // namespace facebook
