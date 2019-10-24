// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <math.h>
#include "DecayAnimationDriver.h"

namespace react {
namespace uwp {
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

bool DecayAnimationDriver::IsAnimationDone(double currentValue, double currentVelocity) {
  return (std::abs(ToValue() - currentValue) < 0.1);
}

double DecayAnimationDriver::ToValue() {
  auto const startValue = [this]() {
    if (auto const manager = m_manager.lock()) {
      if (auto const valueNode = manager->GetValueAnimatedNode(m_animatedValueTag)) {
        return valueNode->Value();
      }
    }
    return 0.0;
  }();

  return m_startValue + m_velocity / (1 - m_deceleration);
}

} // namespace uwp
} // namespace react
