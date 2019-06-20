#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include <folly/dynamic.h>
#include "NativeAnimatedNodesManager.h"
#include <IReactInstance.h>


namespace react {
  namespace uwp {
    class NativeAnimatedNodesManager;
    class PropsAnimatedNode : public AnimatedNode
    {
    public:
      PropsAnimatedNode(int64_t tag, const folly::dynamic& config, const std::weak_ptr<IReactInstance>& instance, const std::shared_ptr<NativeAnimatedNodesManager>& manager);
      void ConnectToView(int64_t viewTag);
      void DisconnectFromView(int64_t viewTag);
      void RestoreDefaultValues();
      void UpdateView();
      void StartAnimations();
      void DisposeCompletedAnimation(int64_t valueTag);
    private:

      winrt::UIElement GetUIElement();
      std::weak_ptr<IReactInstance> m_instance{};
      std::weak_ptr<NativeAnimatedNodesManager> m_manager{};
      std::map<std::string, int64_t> m_propMapping{};
      folly::dynamic m_propMap{};
      int64_t m_connectedViewTag{ -1 };
      std::unordered_map<int64_t, winrt::CompositionAnimation> m_expressionAnimations{};
      winrt::Numerics::float3 m_rotationAxis{ 0,0,1 };
    };
  }
}
