// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "AnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace react {
namespace uwp {

AnimatedNode::AnimatedNode(
    int64_t tag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : m_tag(tag), m_manager(manager) {}

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
  if (std::find(m_children.begin(), m_children.end(), tag) !=
      m_children.end()) {
    if (const auto manager = m_manager.lock()) {
      return manager->GetAnimatedNode(tag);
    }
  }

  return static_cast<AnimatedNode *>(nullptr);
}
} // namespace uwp
} // namespace react
