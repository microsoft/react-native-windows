// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <math.h>
#include "DecayAnimationDriver.h"

namespace Microsoft::ReactNative {
DecayAnimationDriver::DecayAnimationDriver(
    int64_t id,
    int64_t animatedValueTag,
    const Callback &endCallback,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : CalculatedAnimationDriver(id, animatedValueTag, endCallback, config, manager) {
  m_deceleration = config.find(s_decelerationName).dereference().second.asDouble();
  assert(m_deceleration > 0);
  m_velocity = config.find(s_velocityName).dereference().second.asDouble();
}

std::tuple<float, double> DecayAnimationDriver::GetValueAndVelocityForTime(double time) {
  const auto value =
      m_startValue + m_velocity / (1 - m_deceleration) * (1 - std::exp(-(1 - m_deceleration) * (1000 * time)));
  return std::make_tuple(static_cast<float>(value),
                         42.0f); // we don't need the velocity, so set it to a dummy value
}

bool DecayAnimationDriver::IsAnimationDone(
    double currentValue,
    std::optional<double> previousValue,
    double /*currentVelocity*/) {
  return previousValue.has_value() && std::abs(currentValue - previousValue.value()) < 0.1;
}

} // namespace Microsoft::ReactNative
