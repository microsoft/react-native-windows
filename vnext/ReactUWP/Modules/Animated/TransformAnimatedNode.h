// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"
#include "NativeAnimatedNodeManager.h"
#include <folly/dynamic.h>
#include "FacadeType.h"

namespace react {
  namespace uwp {
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
      TransformAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager);
      std::unordered_map<FacadeType, int64_t> GetMapping();
    private:
      std::weak_ptr<NativeAnimatedNodeManager> m_manager;
      std::vector<TransformConfig> m_transformConfigs;
    };
  }
}
