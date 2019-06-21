// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DecayAnimationDriver.h"
#include <jsi/jsi.h>
#include <math.h>

namespace react { namespace uwp {
  DecayAnimationDriver::DecayAnimationDriver(int64_t id, const std::shared_ptr<ValueAnimatedNode>& animatedValue, const Callback& endCallback, const folly::dynamic& config) : AnimationDriver(id, animatedValue, endCallback)
  {
    m_deceleration = config.find("deceleration").dereference().second.asDouble();
    m_velocity = config.find("velocity").dereference().second.asDouble();
    m_iterations = [iterations = config.find("iterations"), end = config.items().end()]() {
      if (iterations != end)
      {
        return iterations.dereference().second.getInt();
      }
      return static_cast<int64_t>(1);
    }();

    const auto [animation, scopedBatch] = [deceleration = m_deceleration, velocity = m_velocity, iterations = m_iterations, fromValue = m_animatedValue->RawValue()]()
    {
      const auto [scopedBatch, animation] = []()
      {
        const auto compositor = winrt::Window::Current().Compositor();
        return std::make_tuple(compositor.CreateScopedBatch(winrt::CompositionBatchTypes::AllAnimations), compositor.CreateScalarKeyFrameAnimation());
      }();

      std::chrono::milliseconds duration(static_cast<int>(velocity / -deceleration * 1000));
      animation.Duration(duration);

      const auto compositor = winrt::Window::Current().Compositor();
      animation.SetScalarParameter(L"velocity", static_cast<float>(velocity));
      animation.SetScalarParameter(L"deceleration", static_cast<float>(deceleration));
      animation.SetScalarParameter(L"duration", static_cast<float>(velocity / -deceleration) * 1000);
      animation.InsertExpressionKeyFrame(1.0f, L"(duration * velocity) + (0.5 * deceleration * duration * duration)", compositor.CreateCubicBezierEasingFunction({ 0,1 }, {0, 1}));
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
