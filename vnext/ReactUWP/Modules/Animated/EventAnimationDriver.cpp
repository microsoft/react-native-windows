#include "pch.h"
#include "EventAnimationDriver.h"
#include <jsi/jsi.h>

namespace react {
  namespace uwp {
    EventAnimationDriver::EventAnimationDriver(const folly::dynamic& eventPath, const std::shared_ptr<ValueAnimatedNode>& animatedValue) : m_animatedValue(animatedValue)
    {
      for (auto path : eventPath)
      {
        m_eventPath.push_back(path.getString());
      }
    }
  }
}
