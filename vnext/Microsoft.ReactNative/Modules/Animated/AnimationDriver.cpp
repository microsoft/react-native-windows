// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Composition.h>
#include "AnimatedPlatformConfig.h"
#include "AnimationDriver.h"

namespace Microsoft::ReactNative {

AnimationDriver::AnimationDriver(
    int64_t id,
    int64_t animatedValueTag,
    const Callback &endCallback,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : m_id(id),
      m_animatedValueTag(animatedValueTag),
      m_config(config.Copy()),
      m_endCallback(endCallback),
      m_manager(manager) {
  m_iterations = config.find("iterations") == config.end() ? 1 : config["iterations"].AsInt64();
  m_useComposition = AnimatedPlatformConfig::ShouldUseComposition(config);
}

void AnimationDriver::DoCallback(bool value) {
  if (m_endCallback) {
    m_endCallback(value);
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
  if (m_useComposition && m_scopedBatch)
    m_scopedBatch.Completed(m_scopedBatchCompletedToken);
}

void AnimationDriver::StartAnimation() {
  assert(m_useComposition);
  m_started = true;
  const auto [animation, scopedBatch] = MakeAnimation(m_config);
  if (auto const animatedValue = GetAnimatedValue()) {
    animatedValue->PropertySet().StartAnimation(ValueAnimatedNode::s_valueName, animation);
    animatedValue->AddActiveAnimation(m_id);
  }
  scopedBatch.End();

  m_scopedBatchCompletedToken = scopedBatch.Completed(
      [weakSelf = weak_from_this(), weakManager = m_manager, id = m_id, tag = m_animatedValueTag](auto sender, auto) {
        const auto strongSelf = weakSelf.lock();
        const auto ignoreCompletedHandlers = strongSelf && strongSelf->m_ignoreCompletedHandlers;
        if (auto manager = weakManager.lock()) {
          // If the animation was stopped for a tracking node, do not clean up the active animation state.
          if (!ignoreCompletedHandlers) {
            if (const auto animatedValue = manager->GetValueAnimatedNode(tag)) {
              animatedValue->RemoveActiveAnimation(id);
            }
            manager->RemoveActiveAnimation(id);
          }

          // Always update the stopped animations in case any animations are deferred for the same value.
          manager->RemoveStoppedAnimation(id, manager);
        }

        if (strongSelf && !ignoreCompletedHandlers) {
          strongSelf->DoCallback(!strongSelf->m_stopped);
        }
      });

  m_animation = animation;
  m_scopedBatch = scopedBatch;
}

void AnimationDriver::StopAnimation(bool ignoreCompletedHandlers) {
  if (m_useComposition && m_started) {
    if (const auto animatedValue = GetAnimatedValue()) {
      animatedValue->PropertySet().StopAnimation(ValueAnimatedNode::s_valueName);
      m_stopped = true;
      m_ignoreCompletedHandlers = ignoreCompletedHandlers;
    }
  } else {
    // For composition animations, the animation may have been deferred and
    // never started. In this case, we will never get a scoped batch
    // completion, so we need to fire the callback synchronously. For rendering
    // animations, we always fire the callback synchronously.
    DoCallback(false);
  }
}

void AnimationDriver::RunAnimationStep(winrt::TimeSpan renderingTime) {
  assert(!m_useComposition);
  if (m_isComplete) {
    return;
  }

  // winrt::TimeSpan ticks are 100 nanoseconds, divide by 10000 to get milliseconds.
  const auto frameTimeMs = renderingTime.count() / 10000.0;
  auto restarting = false;
  if (m_startFrameTimeMs < 0) {
    m_startFrameTimeMs = frameTimeMs;
    restarting = true;
  }

  const auto timeDeltaMs = frameTimeMs - m_startFrameTimeMs;
  const auto isComplete = Update(timeDeltaMs, restarting);

  if (isComplete) {
    if (m_iterations == -1 || ++m_iteration < m_iterations) {
      m_startFrameTimeMs = -1;
    } else {
      m_isComplete = true;
    }
  }
}

ValueAnimatedNode *AnimationDriver::GetAnimatedValue() {
  if (auto manager = m_manager.lock()) {
    return manager->GetValueAnimatedNode(m_animatedValueTag);
  }
  return nullptr;
}
} // namespace Microsoft::ReactNative
