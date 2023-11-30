// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageRequest.h>

namespace facebook::react {

ImageRequest::ImageRequest(
    ImageSource imageSource,
    std::shared_ptr<const ImageTelemetry> telemetry,
    SharedFunction<> cancelationFunction)
    : imageSource_(std::move(imageSource)),
      telemetry_(std::move(telemetry)),
      cancelRequest_(std::move(cancelationFunction)) {
  coordinator_ = std::make_shared<ImageResponseObserverCoordinator>();
}

void ImageRequest::cancel() const {
  cancelRequest_();
}

const ImageSource &ImageRequest::getImageSource() const {
  return imageSource_;
}

const std::shared_ptr<const ImageTelemetry> &ImageRequest::getSharedTelemetry() const {
  return telemetry_;
}

const ImageResponseObserverCoordinator &ImageRequest::getObserverCoordinator() const {
  return *coordinator_;
}

const std::shared_ptr<const ImageResponseObserverCoordinator> &ImageRequest::getSharedObserverCoordinator() const {
  return coordinator_;
}

} // namespace facebook::react
