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
    : AnimatedNode(tag, manager) {
  m_animationId = static_cast<int64_t>(config[s_animationIdName].AsDouble());
  m_toValueId = static_cast<int64_t>(config[s_toValueIdName].AsDouble());
  m_valueId = static_cast<int64_t>(config[s_valueIdName].AsDouble());
  m_animationConfig = std::move(config[s_animationConfigName].AsObject().Copy());

  StartAnimation();
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
      toValueNode->AddActiveTrackingNode(m_tag);
      m_animationConfig[s_toValueIdName] = toValueNode->Value();
      strongManager->StartTrackingAnimatedNode(
          m_animationId, m_valueId, m_toValueId, m_animationConfig, nullptr, strongManager);
    }
  }
}

} // namespace Microsoft::ReactNative
