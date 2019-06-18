#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "NativeAnimatedNodesManager.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class NativeAnimatedNodesManager;
    class DiffClampAnimatedNode : public ValueAnimatedNode
    {
    public:
      DiffClampAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager);

    private:
      int64_t m_inputNodeTag{};
      double m_min{};
      double m_max{};
    };
  }
}
