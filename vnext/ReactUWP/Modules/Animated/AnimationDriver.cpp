// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "AnimationDriver.h"

namespace react {
namespace uwp {

AnimationDriver::AnimationDriver(
    int64_t id,
    int64_t animatedValueTag,
    const Callback &endCallback,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : m_id(id), m_animatedValueTag(animatedValueTag), m_endCallback(endCallback), m_config(config), m_manager(manager) {
  m_iterations = [iterations = config.find("iterations"), end = config.items().end()]() {
    if (iterations != end) {
      return static_cast<int64_t>(iterations.dereference().second.asDouble());
    }
    return static_cast<int64_t>(1);
  }();
}

void AnimationDriver::DoCallback(bool value) {
  if (m_endCallback) {
    m_endCallback(std::vector<folly::dynamic>{folly::dynamic::object("finished", value)});
  }
#ifdef DEBUG
  m_debug_callbackAttempts++;
  if (m_debug_callbackAttempts > 1) {
    throw std::string("The \"finished\" callback for a native animation was called more than once.");
  }
#endif //  DEBUG
  m_endCallback = {};
}

AnimationDriver::~AnimationDriver() {
  if (m_scopedBatch)
    m_scopedBatch.Completed(m_scopedBatchCompletedToken);
}

void AnimationDriver::StartAnimation() {
  const auto [animation, scopedBatch] = MakeAnimation(m_config);

  if (auto const animatedValue = GetAnimatedValue()) {
    auto const previousValue = animatedValue->Value();
    auto const rawValue = animatedValue->RawValue();
    auto const offsetValue = animatedValue->Offset();

    animatedValue->PropertySet().StartAnimation(ValueAnimatedNode::s_offsetName, animation);
    animatedValue->AddActiveAnimation(m_id);
  }
  scopedBatch.End();

  m_scopedBatchCompletedToken = scopedBatch.Completed([&](auto sender, auto) {
    DoCallback(true);
    if (auto manager = m_manager.lock()) {
      if (auto const animatedValue = manager->GetValueAnimatedNode(m_animatedValueTag)) {
        animatedValue->RemoveActiveAnimation(m_id);
        animatedValue->FlattenOffset();
      }
      manager->RemoveActiveAnimation(m_id);
    }
  });

  m_animation = animation;
  m_scopedBatch = scopedBatch;
}

void AnimationDriver::StopAnimation(bool ignoreCompletedHandlers) {
  if (const auto animatedValue = GetAnimatedValue()) {
    animatedValue->PropertySet().StopAnimation(ValueAnimatedNode::s_offsetName);
    if (!ignoreCompletedHandlers) {
      animatedValue->RemoveActiveAnimation(m_id);

      if (m_scopedBatch) {
        DoCallback(false);
        m_scopedBatch.Completed(m_scopedBatchCompletedToken);
        m_scopedBatch = nullptr;
      }
    }
  }
}

ValueAnimatedNode *AnimationDriver::GetAnimatedValue() {
  if (auto manager = m_manager.lock()) {
    return manager->GetValueAnimatedNode(m_animatedValueTag);
  }
  return nullptr;
}
} // namespace uwp
} // namespace react
