// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ValueAnimatedNode.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  typedef std::function<void(std::vector<folly::dynamic>)> Callback;

  class ValueAnimatedNode;
  class AnimationDriver
  {
  public:
    AnimationDriver(int64_t id, const std::shared_ptr<ValueAnimatedNode>& animatedValue, const Callback& endCallback, const folly::dynamic& config);
    void StartAnimation();
    void StopAnimation();

    virtual std::tuple<winrt::CompositionAnimation, winrt::CompositionScopedBatch> MakeAnimation(const folly::dynamic& config)
    {
      return std::make_tuple(nullptr, nullptr);
    };

    inline constexpr int64_t Id() { return m_id; };

  protected:
    int64_t m_id{ 0 };
    std::shared_ptr<ValueAnimatedNode> m_animatedValue{};
    Callback m_endCallback{};
    int64_t m_iterations{ 0 };
    folly::dynamic m_config{};

    winrt::Windows::UI::Composition::CompositionAnimation m_animation{ nullptr };
    //auto revoker for scopedBatch.Completed is broken, tracked by internal bug #22399779
    winrt::event_token m_scopedBatchCompletedToken{};
  };
} }
