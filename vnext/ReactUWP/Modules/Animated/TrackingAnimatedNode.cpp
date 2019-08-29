// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedNodeManager.h"
#include "TrackingAnimatedNode.h"

namespace react {
namespace uwp {
TrackingAnimatedNode::TrackingAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, manager) {
  m_animationId = static_cast<int64_t>(
      config.find(s_animationIdName).dereference().second.asDouble());
  m_toValueId = static_cast<int64_t>(
      config.find(s_toValueIdName).dereference().second.asDouble());
  m_valueId = static_cast<int64_t>(
      config.find(s_valueIdName).dereference().second.asDouble());
  m_animationConfig =
      std::move(config.find(s_animationConfigName).dereference().second);

  StartAnimation();
}

void TrackingAnimatedNode::Update() {
  StartAnimation();
};

void TrackingAnimatedNode::StartAnimation() {
  if (auto const strongManager = m_manager.lock()) {
    if (auto const toValueNode =
            strongManager->GetValueAnimatedNode(m_toValueId)) {
      toValueNode->AddActiveTrackingNode(m_tag);
      m_animationConfig.insert(
          static_cast<folly::StringPiece>(s_toValueIdName),
          toValueNode->Value());
      //m_animationConfig.at("frames").insert()
      strongManager->StartTrackingAnimatedNode(
          m_animationId,
          m_valueId,
          m_toValueId,
          m_animationConfig,
          nullptr,
          strongManager);
    }
  }
}

} // namespace uwp
} // namespace react
