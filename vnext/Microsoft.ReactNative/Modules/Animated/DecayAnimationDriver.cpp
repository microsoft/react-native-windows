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
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : CalculatedAnimationDriver(id, animatedValueTag, endCallback, config, manager) {
  m_deceleration = config[s_decelerationName].AsDouble();
  assert(m_deceleration > 0);
  m_velocity = config[s_velocityName].AsDouble();
}

std::tuple<float, double> DecayAnimationDriver::GetValueAndVelocityForTime(double time) {
  const auto value = m_originalValue.value() +
      m_velocity / (1 - m_deceleration) * (1 - std::exp(-(1 - m_deceleration) * (1000 * time)));
  return std::make_tuple(static_cast<float>(value),
                         42.0f); // we don't need the velocity, so set it to a dummy value
}

bool DecayAnimationDriver::IsAnimationDone(
    double currentValue,
    std::optional<double> previousValue,
    double /*currentVelocity*/) {
  return previousValue.has_value() && std::abs(currentValue - previousValue.value()) < 0.1;
}

bool DecayAnimationDriver::Update(double timeDeltaMs, bool restarting) {
  if (const auto node = GetAnimatedValue()) {
    if (restarting) {
      const auto value = node->RawValue();
      if (!m_originalValue) {
        // First iteration, assign m_fromValue based on AnimatedValue
        m_originalValue = value;
      } else {
        // Not the first iteration, reset AnimatedValue based on m_originalValue
        node->RawValue(m_originalValue.value());
      }

      m_lastValue = value;
    }

    const auto [value, velocity] = GetValueAndVelocityForTime(timeDeltaMs / 1000.0);
    if (restarting || IsAnimationDone(value, m_lastValue, 0.0 /* ignored */)) {
      m_lastValue = value;
      node->RawValue(value);
      return false;
    }
  }

  return true;
}

} // namespace Microsoft::ReactNative
