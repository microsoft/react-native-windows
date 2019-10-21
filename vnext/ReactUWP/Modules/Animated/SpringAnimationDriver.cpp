// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <jsi/jsi.h>
#include <math.h>
#include "SpringAnimationDriver.h"

namespace react {
namespace uwp {
SpringAnimationDriver::SpringAnimationDriver(
    int64_t id,
    int64_t animatedValueTag,
    const Callback &endCallback,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager,
    const folly::dynamic &dynamicToValues)
    : m_dynamicToValues(dynamicToValues),
      AnimationDriver(id, animatedValueTag, endCallback, config, manager) {
  m_springStiffness = config.find(s_springStiffnessParameterName)
                          .dereference()
                          .second.asDouble();
  m_springDamping =
      config.find(s_springDampingParameterName).dereference().second.asDouble();
  m_springMass =
      config.find(s_springMassParameterName).dereference().second.asDouble();
  m_initialVelocity = config.find(s_initialVelocityParameterName)
                          .dereference()
                          .second.asDouble();
  m_endValue =
      config.find(s_endValueParameterName).dereference().second.asDouble();
  m_restSpeedThreshold = config.find(s_restSpeedThresholdParameterName)
                             .dereference()
                             .second.asDouble();
  m_displacementFromRestThreshold =
      config.find(s_displacementFromRestThresholdParameterName)
          .dereference()
          .second.asDouble();
  m_overshootClampingEnabled =
      config.find(s_overshootClampingEnabledParameterName)
          .dereference()
          .second.asBool();
  m_iterations = static_cast<int>(
      config.find(s_iterationsParameterName).dereference().second.asDouble());
}

std::tuple<winrt::CompositionAnimation, winrt::CompositionScopedBatch>
SpringAnimationDriver::MakeAnimation(const folly::dynamic &config) {
  const auto [scopedBatch, animation, easingFunction] = []() {
    const auto compositor = winrt::Window::Current().Compositor();
    return std::make_tuple(
        compositor.CreateScopedBatch(
            winrt::CompositionBatchTypes::AllAnimations),
        compositor.CreateScalarKeyFrameAnimation(),
        compositor.CreateLinearEasingFunction());
  }();

  const auto startValue = GetAnimatedValue()->Value();
  std::vector<float> keyFrames = [this, startValue]() {
    std::vector<float> keyFrames;
    bool done = false;
    double time = 0;
    while (!done) {
      time += 1.0f / 60.0f;
      auto [currentValue, currentVelocity] =
          GetValueAndVelocityForTime(time, startValue);
      keyFrames.push_back(currentValue);
      if (IsAtRest(currentVelocity, currentValue, m_endValue) ||
          (m_overshootClampingEnabled &&
           IsOvershooting(currentValue, startValue))) {
        done = true;
      }
    }
    return keyFrames;
  }();

  std::chrono::milliseconds duration(
      static_cast<int>(keyFrames.size() / 60.0f * 1000.0f));
  animation.Duration(duration);

  auto normalizedProgress = 0.0f;
  // We are animating the values offset property which should start at 0.
  animation.InsertKeyFrame(normalizedProgress, 0.0f, easingFunction);
  for (const auto keyFrame : keyFrames) {
    normalizedProgress =
        std::min(normalizedProgress + 1.0f / keyFrames.size(), 1.0f);
    animation.InsertKeyFrame(
        normalizedProgress,
        keyFrame - static_cast<float>(startValue),
        easingFunction);
  }

  if (m_iterations == -1) {
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Forever);
  } else {
    animation.IterationCount(static_cast<int32_t>(m_iterations));
    animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);
  }

  return std::make_tuple(animation, scopedBatch);
}

std::tuple<float, double> SpringAnimationDriver::GetValueAndVelocityForTime(
    double time,
    double startValue) {
  const auto toValue = [this, time, startValue]() {
    const auto frameFromTime = static_cast<int>(time * 60.0);
    if (frameFromTime < static_cast<int>(m_dynamicToValues.size())) {
      return startValue +
          (m_dynamicToValues[frameFromTime].asDouble() *
           (m_endValue - startValue));
    }
    return m_endValue;
  }();
  const auto c = m_springDamping;
  const auto m = m_springMass;
  const auto k = m_springStiffness;
  const auto v0 = -m_initialVelocity;

  const auto zeta = c / (2 * std::sqrt(k * m));
  const auto omega0 = std::sqrt(k / m);
  const auto omega1 = omega0 * std::sqrt(1.0 - (zeta * zeta));
  const auto x0 = toValue - startValue;

  if (zeta < 1) {
    const auto envelope = std::exp(-zeta * omega0 * time);
    const auto value = static_cast<float>(
        toValue -
        envelope *
            ((v0 + zeta * omega0 * x0) / omega1 * std::sin(omega1 * time) +
             x0 * std::cos(omega1 * time)));
    const auto velocity = zeta * omega0 * envelope *
            (std::sin(omega1 * time) * (v0 + zeta * omega0 * x0) / omega1 +
             x0 * std::cos(omega1 * time)) -
        envelope *
            (std::cos(omega1 * time) * (v0 + zeta * omega0 * x0) -
             omega1 * x0 * std::sin(omega1 * time));
    return std::make_tuple(value, velocity);
  } else {
    const auto envelope = std::exp(-omega0 * time);
    const auto value = static_cast<float>(
        toValue - envelope * (x0 + (v0 + omega0 * x0) * time));
    const auto velocity =
        envelope * (v0 * (time * omega0 - 1) + time * x0 * (omega0 * omega0));
    return std::make_tuple(value, velocity);
  }
}

bool SpringAnimationDriver::IsAtRest(
    double currentVelocity,
    double currentValue,
    double endValue) {
  return std::abs(currentVelocity) <= m_restSpeedThreshold &&
      (std::abs(currentValue - endValue) <= m_displacementFromRestThreshold ||
       m_springStiffness == 0);
}

bool SpringAnimationDriver::IsOvershooting(
    double currentValue,
    double startValue) {
  return m_springStiffness > 0 &&
      ((startValue < m_endValue && currentValue > m_endValue) ||
       (startValue > m_endValue && currentValue < m_endValue));
}

double SpringAnimationDriver::ToValue() {
  return m_endValue;
}

} // namespace uwp
} // namespace react
