// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageRequest.h>

namespace facebook {
namespace react {

ImageRequest::ImageRequest(const ImageSource &imageSource, std::shared_ptr<const ImageTelemetry> telemetry)
    : imageSource_(imageSource), telemetry_(telemetry) {
  // Not implemented.
}

ImageRequest::ImageRequest(ImageRequest &&other) noexcept
    : imageSource_(std::move(other.imageSource_)),
      telemetry_(std::move(other.telemetry_)),
      coordinator_(std::move(other.coordinator_)) {
  // Not implemented.
}

ImageRequest::~ImageRequest() {
  // Not implemented.
}

const ImageResponseObserverCoordinator &ImageRequest::getObserverCoordinator() const {
  // Not implemented
  abort();
}

const std::shared_ptr<const ImageResponseObserverCoordinator> &ImageRequest::getSharedObserverCoordinator() const {
  // Not implemented
  abort();
}

} // namespace react
} // namespace facebook
