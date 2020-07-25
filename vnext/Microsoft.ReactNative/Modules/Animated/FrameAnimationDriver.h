// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "AnimationDriver.h"

namespace react::uwp {
class FrameAnimationDriver : public AnimationDriver {
 public:
  FrameAnimationDriver(
      int64_t id,
      int64_t animatedValueTag,
      const Callback &endCallback,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> MakeAnimation(
      const folly::dynamic &config) override;

  double ToValue() override;

  inline std::vector<double> Frames() override {
    return m_frames;
  }

 private:
  std::vector<double> m_frames{};
  double m_toValue{0};
};
} // namespace react::uwp
