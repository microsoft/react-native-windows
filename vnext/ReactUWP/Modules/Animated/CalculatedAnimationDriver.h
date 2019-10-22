// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include <utility>

namespace react {
namespace uwp {
class CalculatedAnimationDriver : public AnimationDriver {
 public:
   using AnimationDriver::AnimationDriver;

  std::tuple<winrt::CompositionAnimation, winrt::CompositionScopedBatch>
  MakeAnimation(const folly::dynamic &config) override;

 protected:
  virtual std::tuple<float, double> GetValueAndVelocityForTime(
      double time) = 0;

  virtual bool IsAnimationDone(double currentValue, double currentVelocity) = 0;
  double m_startValue{0};
};
} // namespace uwp
} // namespace react
