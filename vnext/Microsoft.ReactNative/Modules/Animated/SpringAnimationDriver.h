#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "CalculatedAnimationDriver.h"

namespace react::uwp {
class SpringAnimationDriver : public CalculatedAnimationDriver {
 public:
  SpringAnimationDriver(
      int64_t id,
      int64_t animatedValueTag,
      const Callback &endCallback,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager,
      const folly::dynamic &dynamicToValues = folly::dynamic::array());

  double ToValue() override;

 protected:
  std::tuple<float, double> GetValueAndVelocityForTime(double time) override;
  bool IsAnimationDone(double currentValue, double currentVelocity) override;

 private:
  bool IsAtRest(double currentVelocity, double currentPosition, double endValue);
  bool IsOvershooting(double currentValue);

  double m_springStiffness{0};
  double m_springDamping{0};
  double m_springMass{0};
  double m_initialVelocity{0};
  double m_endValue{0};
  double m_restSpeedThreshold{0};
  double m_displacementFromRestThreshold{0};
  bool m_overshootClampingEnabled{0};
  int m_iterations{0};
  folly::dynamic m_dynamicToValues{};

  static constexpr std::string_view s_springStiffnessParameterName{"stiffness"};
  static constexpr std::string_view s_springDampingParameterName{"damping"};
  static constexpr std::string_view s_springMassParameterName{"mass"};
  static constexpr std::string_view s_initialVelocityParameterName{"initialVelocity"};
  static constexpr std::string_view s_endValueParameterName{"toValue"};
  static constexpr std::string_view s_restSpeedThresholdParameterName{"restSpeedThreshold"};
  static constexpr std::string_view s_displacementFromRestThresholdParameterName{"restDisplacementThreshold"};
  static constexpr std::string_view s_overshootClampingEnabledParameterName{"overshootClamping"};
  static constexpr std::string_view s_iterationsParameterName{"iterations"};
}; // namespace uwp
} // namespace react::uwp
