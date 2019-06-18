#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "NativeAnimatedNodesManager.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class NativeAnimatedNodesManager;
    class MultiplicationAnimatedNode : public ValueAnimatedNode
    {
    public:
      MultiplicationAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager);

    private:
      std::unordered_set<int64_t> m_inputNodes{};
    };
  }
}
