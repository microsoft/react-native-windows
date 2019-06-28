// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  class FrameAnimationDriver : public AnimationDriver
  {
  public:
    FrameAnimationDriver(int64_t id, int64_t animatedValueTag, const Callback& endCallback, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager);

    std::tuple<winrt::CompositionAnimation, winrt::CompositionScopedBatch> MakeAnimation(const folly::dynamic& config) override;
  private:
    std::vector<double> m_frames {};
    double m_toValue { 0 };
  };
} }
