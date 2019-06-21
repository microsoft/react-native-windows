// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"
#include "NativeAnimatedNodeManager.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  class NativeAnimatedNodeManager;
  class ModulusAnimatedNode : public ValueAnimatedNode
  {
  public:
    ModulusAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager);

  private:
    int64_t m_inputNodeTag{};
    int64_t m_modulus{};
  };
} }
