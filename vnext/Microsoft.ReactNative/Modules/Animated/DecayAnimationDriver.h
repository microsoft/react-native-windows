// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "CalculatedAnimationDriver.h"

namespace Microsoft::ReactNative {
class DecayAnimationDriver : public CalculatedAnimationDriver {
 public:
  DecayAnimationDriver(
      int64_t id,
      int64_t animatedValueTag,
      const Callback &endCallback,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

 protected:
  bool Update(double timeDeltaMs, bool restarting) override;
  std::tuple<float, double> GetValueAndVelocityForTime(double time) override;
  bool IsAnimationDone(double currentValue, std::optional<double> previousValue, double currentVelocity) override;

 private:
  double m_velocity{0};
  double m_deceleration{0};
  double m_lastValue{0};

  static constexpr std::string_view s_velocityName{"velocity"};
  static constexpr std::string_view s_decelerationName{"deceleration"};

  static constexpr std::wstring_view s_velocityParameterName{L"velocity"};
  static constexpr std::wstring_view s_decelerationParameterName{L"deceleration"};
  static constexpr std::wstring_view s_durationName{L"duration"};
};
} // namespace Microsoft::ReactNative
