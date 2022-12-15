// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include <utility>
#include "AnimatedNode.h"
#include "AnimationDriver.h"

namespace Microsoft::ReactNative {
class CalculatedAnimationDriver : public AnimationDriver {
 public:
  using AnimationDriver::AnimationDriver;

  std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> MakeAnimation(
      const winrt::Microsoft::ReactNative::JSValueObject &config) override;

 protected:
  virtual std::tuple<float, double> GetValueAndVelocityForTime(double time) = 0;
  virtual bool IsAnimationDone(double currentValue, std::optional<double> previousValue, double currentVelocity) = 0;
};
} // namespace Microsoft::ReactNative
