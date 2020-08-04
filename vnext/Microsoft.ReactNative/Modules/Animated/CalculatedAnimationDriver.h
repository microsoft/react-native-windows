// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include <utility>
#include "AnimatedNode.h"
#include "AnimationDriver.h"

namespace react::uwp {
class CalculatedAnimationDriver : public AnimationDriver {
 public:
  using AnimationDriver::AnimationDriver;

  std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> MakeAnimation(
      const folly::dynamic &config) override;

 protected:
  virtual std::tuple<float, double> GetValueAndVelocityForTime(double time) = 0;

  virtual bool IsAnimationDone(double currentValue, double currentVelocity) = 0;
  double m_startValue{0};
};
} // namespace react::uwp
