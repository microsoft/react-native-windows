/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "WindowsTextInputState.h"

#include <react/renderer/components/text/platform/android/react/renderer/components/text/ParagraphState.h> // [windows]

namespace facebook::react {

folly::dynamic WindowsTextInputState::getDynamic() const {
  LOG(FATAL) << "TextInputState state should only be read using MapBuffer";
  return folly::dynamic::object(); // windows
}

} // namespace facebook::react
