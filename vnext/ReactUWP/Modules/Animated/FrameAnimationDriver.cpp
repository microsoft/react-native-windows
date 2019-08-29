// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "FrameAnimationDriver.h"

namespace react {
namespace uwp {
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

std::tuple<winrt::CompositionAnimation, winrt::CompositionScopedBatch>
FrameAnimationDriver::MakeAnimation(const folly::dynamic &config) {
  const auto [scopedBatch, animation] = []() {
    const auto compositor = winrt::Window::Current().Compositor();
    return std::make_tuple(
        compositor.CreateScopedBatch(
            winrt::CompositionBatchTypes::AllAnimations),
        compositor.CreateScalarKeyFrameAnimation());
  }();

  // Frames contains 60 values per second of duration of the animation, convert
  // the size of frames to duration in ms.
  std::chrono::milliseconds duration(
      static_cast<int>(m_frames.size() * 1000.0 / 60.0));
  animation.Duration(duration);

  auto normalizedProgress = 0.0f;
  auto step = 1.0f / m_frames.size();
  auto fromValue = GetAnimatedValue()->RawValue();
  for (auto frame : m_frames) {
    normalizedProgress = std::min(normalizedProgress += step, 1.0f);
    animation.InsertKeyFrame(
        normalizedProgress,
        static_cast<float>(fromValue + (frame * (m_toValue - fromValue))));
  }

  animation.IterationCount(static_cast<int32_t>(m_iterations));
  animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);

  return std::make_tuple(animation, scopedBatch);
}

double FrameAnimationDriver::ToValue() {
  return m_toValue;
}

} // namespace uwp
} // namespace react
