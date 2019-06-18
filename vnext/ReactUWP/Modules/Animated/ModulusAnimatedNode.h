#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "NativeAnimatedNodesManager.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class NativeAnimatedNodesManager;
    class ModulusAnimatedNode : public ValueAnimatedNode
    {
    public:
      ModulusAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager);

    private:
      int64_t m_inputNodeTag{};
      int64_t m_modulus{};
    };
  }
}
