// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AnimatedNode.h"
#include "AnimatedPlatformConfig.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {

AnimatedNode::AnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : m_tag(tag), m_manager(manager) {
  m_useComposition = AnimatedPlatformConfig::ShouldUseComposition(config);
}

int64_t AnimatedNode::Tag() {
  return m_tag;
}

void AnimatedNode::AddChild(const int64_t animatedNodeTag) {
  m_children.push_back(animatedNodeTag);
  GetChildNode(animatedNodeTag)->OnAttachToNode(m_tag);
}

void AnimatedNode::RemoveChild(const int64_t tag) {
  if (const auto childNode = GetChildNode(tag)) {
    childNode->OnDetachedFromNode(m_tag);
    m_children.erase(std::find(m_children.begin(), m_children.end(), tag));
  }
}

AnimatedNode *AnimatedNode::GetChildNode(int64_t tag) {
  if (std::find(m_children.begin(), m_children.end(), tag) != m_children.end()) {
    if (const auto manager = m_manager.lock()) {
      return manager->GetAnimatedNode(tag);
    }
  }

  return static_cast<AnimatedNode *>(nullptr);
}

bool AnimatedNode::HasCompatibleAnimationDriver(int64_t tag) {
#if DEBUG
  if (const auto manager = m_manager.lock()) {
    return manager->GetAnimatedNode(tag)->UseComposition() == m_useComposition;
  }
#endif
  return true;
}
} // namespace Microsoft::ReactNative
