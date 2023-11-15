/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "WindowsModalHostViewState.h"

namespace facebook::react {

#ifdef ANDROID
folly::dynamic WindowsModalHostViewState::getDynamic() const {
  return folly::dynamic::object("screenWidth", screenSize.width)(
      "screenHeight", screenSize.height);
}
#endif

} // namespace facebook::react