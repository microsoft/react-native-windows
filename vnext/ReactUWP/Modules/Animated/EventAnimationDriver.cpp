// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <jsi/jsi.h>
#include "EventAnimationDriver.h"
#include "NativeAnimatedNodeManager.h"


namespace react {
namespace uwp {
EventAnimationDriver::EventAnimationDriver(
    const folly::dynamic &eventPath,
    int64_t animatedValueTag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : m_animatedValueTag(animatedValueTag), m_manager(manager) {
  for (const auto &path : eventPath) {
    m_eventPath.push_back(path.getString());
  }
}

ValueAnimatedNode *EventAnimationDriver::AnimatedValue() {
  if (const auto manager = m_manager.lock()) {
    return manager->GetValueAnimatedNode(m_animatedValueTag);
  }
  return static_cast<ValueAnimatedNode *>(nullptr);
}

} // namespace uwp
} // namespace react
