// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"
#include <folly/dynamic.h>
#include "NativeAnimatedNodeManager.h"

namespace react { namespace uwp {
  class StyleAnimatedNode : public AnimatedNode
  {
  public:
    StyleAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager);
    void CollectViewUpdates(const folly::dynamic& propsMap);

    std::unordered_map<FacadeType, int64_t> GetMapping();
  private:
    std::weak_ptr<NativeAnimatedNodeManager> m_manager{};
    std::map<std::string, int64_t> m_propMapping{};
  };
} }
