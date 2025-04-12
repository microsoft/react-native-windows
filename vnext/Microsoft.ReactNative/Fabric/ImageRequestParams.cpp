// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/graphics/Float.h>

namespace facebook::react {

class ImageRequestParams {
 public:
  ImageRequestParams() = default;
  explicit ImageRequestParams(Float blurRadius) : blurRadius(blurRadius) {}

  Float blurRadius{};

  bool operator==(const ImageRequestParams &rhs) const {
    return this->blurRadius == rhs.blurRadius;
  }

  bool operator!=(const ImageRequestParams &rhs) const {
    return !(*this == rhs);
  }
};

} // namespace facebook::react
