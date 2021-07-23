// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "FrameAnimationDriver.h"
#include "Utils/Helpers.h"

namespace Microsoft::ReactNative {
FrameAnimationDriver::FrameAnimationDriver(
    int64_t id,
    int64_t animatedValueTag,
    const Callback &endCallback,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimationDriver(id, animatedValueTag, endCallback, config, manager) {
  for (const auto &frame : config.find("frames").dereference().second) {
    m_frames.push_back(frame.asDouble());
  }
  m_toValue = config.find("toValue").dereference().second.asDouble();
}

std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> FrameAnimationDriver::MakeAnimation(
    const folly::dynamic & /*config*/) {
  const auto [scopedBatch, animation] = []() {
    const auto compositor = Microsoft::ReactNative::GetCompositor();
    return std::make_tuple(
        compositor.CreateScopedBatch(
            IsRS5OrHigher() ? comp::CompositionBatchTypes::AllAnimations : comp::CompositionBatchTypes::Animation),
        compositor.CreateScalarKeyFrameAnimation());
  }();

  // Frames contains 60 values per second of duration of the animation, convert
  // the size of frames to duration in ms.
  std::chrono::milliseconds duration(static_cast<int>(m_frames.size() * 1000.0 / 60.0));
  animation.Duration(duration);

  auto normalizedProgress = 0.0f;
  auto step = 1.0f / m_frames.size();
  auto fromValue = GetAnimatedValue()->RawValue();
  for (auto frame : m_frames) {
    normalizedProgress = std::min(normalizedProgress += step, 1.0f);
    animation.InsertKeyFrame(normalizedProgress, static_cast<float>(frame * (m_toValue - fromValue)));
  }

  if (m_iterations == -1) {
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Forever);
  } else {
    animation.IterationCount(static_cast<int32_t>(m_iterations));
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);
  }

  return std::make_tuple(animation, scopedBatch);
}

double FrameAnimationDriver::ToValue() {
  return m_toValue;
}

} // namespace Microsoft::ReactNative
