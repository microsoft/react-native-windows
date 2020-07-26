// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "NativeAnimatedNodeManager.h"
#include "ValueAnimatedNode.h"

namespace react::uwp {
typedef std::function<void(std::vector<folly::dynamic>)> Callback;

class ValueAnimatedNode;
class AnimationDriver {
 public:
  AnimationDriver(
      int64_t id,
      int64_t animatedValueTag,
      const Callback &endCallback,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  virtual ~AnimationDriver();
  void StartAnimation();
  void StopAnimation(bool ignoreCompletedHandlers = false);

  virtual std::tuple<comp::CompositionAnimation, comp::CompositionScopedBatch> MakeAnimation(
      const folly::dynamic & /*config*/) {
    return std::make_tuple(nullptr, nullptr);
  };

  inline constexpr int64_t Id() {
    return m_id;
  };

  inline constexpr int64_t AnimatedValueTag() {
    return m_animatedValueTag;
  }

  inline Callback EndCallback() {
    return m_endCallback;
  }

  inline folly::dynamic AnimationConfig() {
    return m_config;
  }

  virtual double ToValue() {
    return 0;
  };

  virtual std::vector<double> Frames() {
    return std::vector<double>();
  }

 private:
  Callback m_endCallback{};
  void DoCallback(bool value);
#ifdef DEBUG
  int m_debug_callbackAttempts{0};
#endif // DEBUG

 protected:
  ValueAnimatedNode *GetAnimatedValue();

  int64_t m_id{0};
  int64_t m_animatedValueTag{};
  int64_t m_iterations{0};
  folly::dynamic m_config{};
  std::weak_ptr<NativeAnimatedNodeManager> m_manager{};

  comp::CompositionAnimation m_animation{nullptr};
  comp::CompositionScopedBatch m_scopedBatch{nullptr};
  // auto revoker for scopedBatch.Completed is broken, tracked by internal bug
  // #22399779
  winrt::event_token m_scopedBatchCompletedToken{};
};
} // namespace react::uwp
