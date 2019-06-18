#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include "NativeAnimatedNodesManager.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {
    enum FacadeType : int;
    struct TransformConfig
    {
    public:
      std::string property;
      int64_t nodeTag;
      double value;
    };

    class TransformAnimatedNode : public AnimatedNode
    {
    public:
      TransformAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager);
      std::unordered_map<FacadeType, int64_t> GetMapping();
    private:
      std::weak_ptr<NativeAnimatedNodesManager> m_manager;
      std::vector<TransformConfig> m_transformConfigs;
    };
  }
}
