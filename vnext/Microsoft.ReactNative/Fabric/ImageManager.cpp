// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageManager.h>
#include "WindowsImageManager.h"

namespace facebook {
namespace react {

ImageManager::ImageManager(ContextContainer::Shared const &contextContainer) {
  auto reactContext = *contextContainer->find<winrt::Microsoft::ReactNative::ReactContext>("MSRN.ReactContext");
  self_ = new Microsoft::ReactNative::WindowsImageManager(reactContext);
}

ImageManager::~ImageManager() {
  delete (Microsoft::ReactNative::WindowsImageManager *)self_;
}

ImageRequest ImageManager::requestImage(const ImageSource &imageSource, SurfaceId surfaceId) const {
  return ((Microsoft::ReactNative::WindowsImageManager *)self_)->requestImage(imageSource, surfaceId);
}

} // namespace react
} // namespace facebook
