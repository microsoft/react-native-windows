// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedNodeManager.h"
#include "TrackingAnimatedNode.h"

namespace Microsoft::ReactNative {
TrackingAnimatedNode::TrackingAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, config, manager) {
  m_animationId = config[s_animationIdName].AsInt64();
  m_toValueId = config[s_toValueIdName].AsInt64();
  m_valueId = config[s_valueIdName].AsInt64();
  m_animationConfig = std::move(config[s_animationConfigName].AsObject().Copy());
  if (config.count(s_platformConfigName) && !m_animationConfig.count(s_platformConfigName)) {
    m_animationConfig[s_platformConfigName] = std::move(config[s_platformConfigName].Copy());
  }

  if (m_useComposition) {
    StartAnimation();
  }
}

void TrackingAnimatedNode::Update() {
  StartAnimation();
};

void TrackingAnimatedNode::StartAnimation() {
  if (auto const strongManager = m_manager.lock()) {
    if (auto const toValueNode = strongManager->GetValueAnimatedNode(m_toValueId)) {
      // In case the animation is already running, we need to stop it to free up the
      // animationId key in the active animations map in the animation manager.
      strongManager->StopAnimation(m_animationId, true);
      m_animationConfig[s_toValueIdName] = toValueNode->Value();
      if (m_useComposition) {
        toValueNode->AddActiveTrackingNode(m_tag);
        strongManager->StartTrackingAnimatedNode(
            m_animationId, m_valueId, m_toValueId, m_animationConfig, nullptr, strongManager);
      } else {
        strongManager->StartAnimatingNode(m_animationId, m_valueId, m_animationConfig, nullptr, strongManager);
      }
    }
  }
}

} // namespace Microsoft::ReactNative
