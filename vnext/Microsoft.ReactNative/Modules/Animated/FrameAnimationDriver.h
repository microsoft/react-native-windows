// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "AnimationDriver.h"

namespace Microsoft::ReactNative {
class FrameAnimationDriver : public AnimationDriver {
 public:
  FrameAnimationDriver(
      int64_t id,
      int64_t animatedValueTag,
      const Callback &endCallback,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> MakeAnimation(
      const winrt::Microsoft::ReactNative::JSValueObject &config) override;

  double ToValue() override;

  inline std::vector<double> Frames() override {
    return m_frames;
  }

 private:
  std::vector<double> m_frames{};
  double m_toValue{0};
};
} // namespace Microsoft::ReactNative
