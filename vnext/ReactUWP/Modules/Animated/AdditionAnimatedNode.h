// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "ValueAnimatedNode.h"

namespace react {
namespace uwp {
class AdditionAnimatedNode : public ValueAnimatedNode {
 public:
  AdditionAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

 private:
  std::unordered_set<int64_t> m_inputNodes{};
};
} // namespace uwp
} // namespace react
