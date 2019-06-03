#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class FrameAnimationDriver : public AnimationDriver
    {
    public:
      FrameAnimationDriver(int64_t id, std::weak_ptr<ValueAnimatedNode> animatedValue, Callback endCallback, const folly::dynamic& config);

    private:
      std::vector<double> m_frames {};
      double m_toValue { 0 };
      int64_t m_iterations { 0 };
    };
  }
}
