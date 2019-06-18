#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "NativeAnimatedNodesManager.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class NativeAnimatedNodesManager;
    class SubtractionAnimatedNode : public ValueAnimatedNode
    {
    public:
      SubtractionAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager);

    private:
      int64_t m_firstInput{ -1 };
      std::unordered_set<int64_t> m_inputNodes{};
    };
  }
}
