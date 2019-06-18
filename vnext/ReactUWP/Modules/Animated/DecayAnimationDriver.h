#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class DecayAnimationDriver : public AnimationDriver
    {
    public:
      DecayAnimationDriver(int64_t id, std::shared_ptr<ValueAnimatedNode> animatedValue, Callback endCallback, const folly::dynamic& config);
    private:
      double m_velocity{ 0 };
      double m_deceleration{ 0 };
    };
  }
}
