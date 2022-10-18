// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "EventAnimationDriver.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {
EventAnimationDriver::EventAnimationDriver(
    const std::vector<std::string> &eventPath,
    int64_t animatedValueTag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : m_animatedValueTag(animatedValueTag), m_manager(manager) {
  for (const auto &path : eventPath) {
    m_eventPath.push_back(path);
  }
}

ValueAnimatedNode *EventAnimationDriver::AnimatedValue() {
  if (const auto manager = m_manager.lock()) {
    return manager->GetValueAnimatedNode(m_animatedValueTag);
  }
  return static_cast<ValueAnimatedNode *>(nullptr);
}

} // namespace Microsoft::ReactNative
