#include "pch.h"
#include "FrameAnimationDriver.h"

namespace react {
  namespace uwp {
    FrameAnimationDriver::FrameAnimationDriver(int64_t id, std::weak_ptr<ValueAnimatedNode> animatedValue, Callback endCallback, const folly::dynamic& config) : AnimationDriver(id, animatedValue, endCallback)
    {
      for (auto frame : config.find("frames").dereference().second)
      {
        m_frames.push_back(frame.asDouble());
      }
      m_toValue = config.find("toValue").dereference().second.asDouble();
      m_iterations = [iterations = config.find("iterations"), end = config.items().end()]() {
        if (iterations != end)
        {
          return iterations.dereference().second.getInt();
        }
        return static_cast<int64_t>(1);
      }();

      m_animation = 
        [frames = m_frames, toValue = m_toValue, iterations = m_iterations](){
          auto animation = winrt::Window::Current().Compositor().CreateScalarKeyFrameAnimation();

          std::chrono::milliseconds duration(static_cast<int>(frames.size() * 1000.0 / 60.0));
          animation.Duration(duration);

          auto normalizedProgress = 0.0f;
          auto step = 1.0f / frames.size();
          for (auto frame : frames)
          {
            normalizedProgress += step;
            animation.InsertKeyFrame(normalizedProgress, static_cast<float>(frame));
          }

          animation.IterationCount(static_cast<int32_t>(iterations));
          animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);

          return animation;
      }();

      if (auto strong_animatedValue = animatedValue.lock())
      {
        strong_animatedValue->PropertySet().StartAnimation(L"offset", m_animation);
      }
    }
  }
}
