#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "NativeAnimatedNodesManager.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class NativeAnimatedNodesManager;
    class ValueAnimatedNode : public AnimatedNode
    {
    public:
      ValueAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager);
      ValueAnimatedNode(int64_t tag, const std::shared_ptr<NativeAnimatedNodesManager>& manager);
      double Value();
      double RawValue();
      void RawValue(float value);
      double Offset();
      void Offset(float offset);
      void FlattenOffset();
      void ExtractOffset();
      winrt::CompositionPropertySet PropertySet() { return m_propertySet; };

      void AddDependentPropsNode(int64_t propsNodeTag);
      void RemoveDependentPropsNode(int64_t propsNodeTag);
      void AddActiveAnimation(int64_t animationTag);
      void RemoveActiveAnimation(int64_t animationTag);

    protected:
      std::weak_ptr<NativeAnimatedNodesManager> m_manager{};
      winrt::CompositionPropertySet m_propertySet{ nullptr };

    private:
      std::unordered_set<int64_t> m_dependentPropsNodes{};
      std::unordered_set<int64_t> m_activeAnimations{};
    };
  }
}
