// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "FrameAnimationDriver.h"
#include <jsi/jsi.h>

namespace react { namespace uwp {
  FrameAnimationDriver::FrameAnimationDriver(int64_t id, const std::shared_ptr<ValueAnimatedNode>& animatedValue, const Callback& endCallback, const folly::dynamic& config) : AnimationDriver(id, animatedValue, endCallback)
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

    const auto [animation, scopedBatch] = [frames = m_frames, toValue = m_toValue, iterations = m_iterations, fromValue = m_animatedValue->RawValue()]()
    {
      const auto [scopedBatch, animation] = []()
      {
        const auto compositor = winrt::Window::Current().Compositor();
        return std::make_tuple(compositor.CreateScopedBatch(winrt::CompositionBatchTypes::AllAnimations), compositor.CreateScalarKeyFrameAnimation());
      }();

      // Frames contains 60 values per second of duration of the animation, convert the size of frames to during in ms.
      std::chrono::milliseconds duration(static_cast<int>(frames.size() * 1000.0 / 60.0));
      animation.Duration(duration);

      auto normalizedProgress = 0.0f;
      auto step = 1.0f / frames.size();
      for (auto frame : frames)
      {
        normalizedProgress += step;
        animation.InsertKeyFrame(normalizedProgress, static_cast<float>(fromValue + (frame * (toValue - fromValue))));
      }

      animation.IterationCount(static_cast<int32_t>(iterations));
      animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);

      return std::make_tuple(animation, scopedBatch);
    }();

    if (m_animatedValue)
    {
      m_animatedValue->PropertySet().StartAnimation(L"offset", animation);
      m_animatedValue->AddActiveAnimation(m_id);
    }
    scopedBatch.End();

    scopedBatch.Completed([endCallback = m_endCallback, animatedValue = m_animatedValue, id = m_id](auto sender, auto)
    {
      if (endCallback)
      {
        endCallback(std::vector<folly::dynamic>{folly::dynamic::object("finished", "true")});
      }
      auto offset = animatedValue->Offset();
      if (animatedValue)
      {
        animatedValue->RemoveActiveAnimation(id);
      }
    });

    m_animation = animation;
  }
} }
