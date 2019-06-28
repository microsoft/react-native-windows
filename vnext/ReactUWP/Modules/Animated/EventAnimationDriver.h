// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"
#include "ValueAnimatedNode.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  class ValueAnimatedNode;
  class EventAnimationDriver
  {
  public:
    EventAnimationDriver(const folly::dynamic& eventPath, int64_t animatedValueTag, const std::shared_ptr<NativeAnimatedNodeManager>& manager);
    ValueAnimatedNode* AnimatedValue();

  private:
    std::vector<std::string> m_eventPath{};
    int64_t m_animatedValueTag{};
    std::weak_ptr<NativeAnimatedNodeManager> m_manager{};
  };
} }
