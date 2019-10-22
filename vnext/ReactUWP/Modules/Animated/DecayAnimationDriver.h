// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "CalculatedAnimationDriver.h"

namespace react {
namespace uwp {
class DecayAnimationDriver : public CalculatedAnimationDriver {
 public:
  DecayAnimationDriver(
      int64_t id,
      int64_t animatedValueTag,
      const Callback &endCallback,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  double ToValue() override;

 protected:
  std::tuple<float, double> GetValueAndVelocityForTime(
      double time) override;
  bool IsAnimationDone(double currentValue, double currentVelocity) override;

 private:
  double m_velocity{0};
  double m_deceleration{0};

  static constexpr std::string_view s_velocityName{"velocity"};
  static constexpr std::string_view s_decelerationName{"deceleration"};

  static constexpr std::wstring_view s_velocityParameterName{L"velocity"};
  static constexpr std::wstring_view s_decelerationParameterName{
      L"deceleration"};
  static constexpr std::wstring_view s_durationName{L"duration"};
};
} // namespace uwp
} // namespace react
