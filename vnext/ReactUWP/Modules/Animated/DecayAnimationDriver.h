// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  class DecayAnimationDriver : public AnimationDriver
  {
  public:
    DecayAnimationDriver(int64_t id, const std::shared_ptr<ValueAnimatedNode>& animatedValue, const Callback& endCallback, const folly::dynamic& config);

    std::tuple<winrt::CompositionAnimation, winrt::CompositionScopedBatch> MakeAnimation(const folly::dynamic& config) override;
  private:
    double m_velocity{ 0 };
    double m_deceleration{ 0 };

    static constexpr std::string_view s_velocityName{ "velocity" };
    static constexpr std::string_view s_decelerationName{ "deceleration" };

    static constexpr std::wstring_view s_velocityParameterName{ L"velocity" };
    static constexpr std::wstring_view s_decelerationParameterName{ L"deceleration" };
    static constexpr std::wstring_view s_durationName{ L"duration" };
  };
} }
