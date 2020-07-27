// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <math.h>
#include "CalculatedAnimationDriver.h"

namespace react::uwp {

std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> CalculatedAnimationDriver::MakeAnimation(
    const folly::dynamic & /*config*/) {
  const auto [scopedBatch, animation, easingFunction] = []() {
    const auto compositor = react::uwp::GetCompositor();
    return std::make_tuple(
        compositor.CreateScopedBatch(comp::CompositionBatchTypes::AllAnimations),
        compositor.CreateScalarKeyFrameAnimation(),
        compositor.CreateLinearEasingFunction());
  }();

  m_startValue = GetAnimatedValue()->Value();
  std::vector<float> keyFrames = [this]() {
    std::vector<float> keyFrames;
    bool done = false;
    double time = 0;
    while (!done) {
      time += 1.0f / 60.0f;
      auto [currentValue, currentVelocity] = GetValueAndVelocityForTime(time);
      keyFrames.push_back(currentValue);
      if (IsAnimationDone(currentValue, currentVelocity)) {
        done = true;
      }
    }
    return keyFrames;
  }();

  std::chrono::milliseconds duration(static_cast<int>(keyFrames.size() / 60.0f * 1000.0f));
  animation.Duration(duration);
  auto normalizedProgress = 0.0f;
  // We are animating the values offset property which should start at 0.
  animation.InsertKeyFrame(normalizedProgress, 0.0f, easingFunction);
  for (const auto keyFrame : keyFrames) {
    normalizedProgress = std::min(normalizedProgress + 1.0f / keyFrames.size(), 1.0f);
    animation.InsertKeyFrame(normalizedProgress, keyFrame - static_cast<float>(m_startValue), easingFunction);
  }

  if (m_iterations == -1) {
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Forever);
  } else {
    animation.IterationCount(static_cast<int32_t>(m_iterations));
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);
  }

  return std::make_tuple(animation, scopedBatch);
}

} // namespace react::uwp
