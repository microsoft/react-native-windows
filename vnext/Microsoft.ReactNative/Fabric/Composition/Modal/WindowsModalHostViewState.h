/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/core/graphicsConversions.h>
#include <react/renderer/graphics/Float.h>

#ifdef ANDROID
#include <folly/dynamic.h>
#include <react/renderer/mapbuffer/MapBuffer.h>
#include <react/renderer/mapbuffer/MapBufferBuilder.h>
#endif

namespace facebook::react {

/*
 * State for <WindowsModalHostView> component.
 */
class WindowsModalHostViewState final {
 public:
  using Shared = std::shared_ptr<const WindowsModalHostViewState>;

  WindowsModalHostViewState(){};
  WindowsModalHostViewState(Size screenSize_) : screenSize(screenSize_){};

#ifdef ANDROID
  WindowsModalHostViewState(const WindowsModalHostViewState &previousState, folly::dynamic data)
      : screenSize(Size{(Float)data["screenWidth"].getDouble(), (Float)data["screenHeight"].getDouble()}){};
#endif

  const Size screenSize{};

#ifdef ANDROID
  folly::dynamic getDynamic() const;
  MapBuffer getMapBuffer() const {
    return MapBufferBuilder::EMPTY();
  };

#endif

#pragma mark - Getters
};

} // namespace facebook::react
