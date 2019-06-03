#pragma once

#include "pch.h"

namespace react {
  namespace uwp {

    class AnimatedNode
    {
    public:
      AnimatedNode(int64_t tag);
      int64_t Tag();
      void AddChild(const std::shared_ptr<AnimatedNode>& animatedNode);
      void RemoveChild(const int64_t animatedNode);

      virtual void Update() {};
      virtual void OnDetachedFromNode(const std::shared_ptr<AnimatedNode>& animatedNode) {};
      virtual void OnAttachToNode(const std::shared_ptr<AnimatedNode>& animatedNode) {};

    protected:
      std::unordered_map<int64_t, std::shared_ptr<AnimatedNode>> m_children{};
      int64_t m_tag{ 0 };
    };
  }
}
