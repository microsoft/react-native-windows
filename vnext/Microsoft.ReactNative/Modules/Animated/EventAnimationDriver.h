// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "ValueAnimatedNode.h"

namespace Microsoft::ReactNative {
class ValueAnimatedNode;
class EventAnimationDriver {
 public:
  EventAnimationDriver(
      const std::vector<std::string> &eventPath,
      int64_t animatedValueTag,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  ValueAnimatedNode *AnimatedValue();

 private:
  std::vector<std::string> m_eventPath{};
  int64_t m_animatedValueTag{};
  std::weak_ptr<NativeAnimatedNodeManager> m_manager{};
};
} // namespace Microsoft::ReactNative
