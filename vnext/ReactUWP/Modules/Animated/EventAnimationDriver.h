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
    EventAnimationDriver(const folly::dynamic& eventPath, const std::shared_ptr<ValueAnimatedNode>& animatedValue);
    std::weak_ptr<ValueAnimatedNode> AnimatedValue() { return m_animatedValue; };

  private:
    std::vector<std::string> m_eventPath{};
    std::weak_ptr<ValueAnimatedNode> m_animatedValue{};
  };
} }
