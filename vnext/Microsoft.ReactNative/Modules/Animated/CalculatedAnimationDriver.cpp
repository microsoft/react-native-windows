// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <math.h>
#include "CalculatedAnimationDriver.h"

namespace Microsoft::ReactNative {

std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> CalculatedAnimationDriver::MakeAnimation(
    const winrt::Microsoft::ReactNative::JSValueObject & /*config*/) {
  assert(m_useComposition);
  const auto [scopedBatch, animation, easingFunction] = [manager = m_manager.lock()]() {
    const auto compositor = manager->Compositor();
    return std::make_tuple(
        compositor.CreateScopedBatch(comp::CompositionBatchTypes::AllAnimations),
        compositor.CreateScalarKeyFrameAnimation(),
        compositor.CreateLinearEasingFunction());
  }();

  m_originalValue = GetAnimatedValue()->RawValue();
  std::vector<float> keyFrames = [this]() {
    std::vector<float> keyFrames;
    bool done = false;
    double time = 0;
    std::optional<double> previousValue = std::nullopt;
    while (!done) {
      time += 1.0f / 60.0f;
      auto [currentValue, currentVelocity] = GetValueAndVelocityForTime(time);
      keyFrames.push_back(currentValue);
      if (IsAnimationDone(currentValue, previousValue, currentVelocity)) {
        done = true;
      }
      previousValue = currentValue;
    }
    return keyFrames;
  }();

  std::chrono::milliseconds duration(static_cast<int>(keyFrames.size() / 60.0f * 1000.0f));
  animation.Duration(duration);
  auto normalizedProgress = 0.0f;
  auto fromValue = static_cast<float>(m_originalValue.value());
  animation.InsertKeyFrame(normalizedProgress, fromValue, easingFunction);
  for (const auto keyFrame : keyFrames) {
    normalizedProgress = std::min(normalizedProgress + 1.0f / keyFrames.size(), 1.0f);
    animation.InsertKeyFrame(normalizedProgress, keyFrame, easingFunction);
  }

  if (m_iterations == -1) {
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Forever);
  } else {
    animation.IterationCount(static_cast<int32_t>(m_iterations));
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);
  }

  return std::make_tuple(animation, scopedBatch);
}

} // namespace Microsoft::ReactNative
