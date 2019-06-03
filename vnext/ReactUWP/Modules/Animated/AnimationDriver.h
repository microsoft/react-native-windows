#pragma once

#include "pch.h"
#include "ValueAnimatedNode.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    typedef std::function<void(std::vector<folly::dynamic>)> Callback;

    class AnimationDriver
    {
    public:
      AnimationDriver(int64_t id, std::weak_ptr<ValueAnimatedNode> animatedValue, Callback endCallback);

      inline constexpr int64_t Id() { return m_id; };

    protected:
      int64_t m_id{ 0 };
      std::weak_ptr<ValueAnimatedNode> m_animatedValue{};
      Callback m_endCallback{};

      winrt::KeyFrameAnimation m_animation{ nullptr };
    };
  }
}
