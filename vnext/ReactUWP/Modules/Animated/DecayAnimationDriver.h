// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  class DecayAnimationDriver : public AnimationDriver
  {
  public:
    DecayAnimationDriver(int64_t id, const std::shared_ptr<ValueAnimatedNode>& animatedValue, const Callback& endCallback, const folly::dynamic& config);
  private:
    double m_velocity{ 0 };
    double m_deceleration{ 0 };
  };
} }
