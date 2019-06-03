// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AnimatedNode.h"

namespace react {
  namespace uwp {

    AnimatedNode::AnimatedNode(int64_t tag)
    {
      m_tag = tag;
    }

    int64_t AnimatedNode::Tag()
    {
      return m_tag;
    }

    void AnimatedNode::AddChild(const std::shared_ptr<AnimatedNode>& animatedNode)
    {
      m_children.insert({ animatedNode->Tag(), animatedNode });
    }

    void AnimatedNode::RemoveChild(const int64_t tag)
    {
      m_children.erase(tag);
    }
  }
}
