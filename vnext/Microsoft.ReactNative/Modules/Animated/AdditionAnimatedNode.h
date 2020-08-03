// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "ValueAnimatedNode.h"

namespace react::uwp {
class AdditionAnimatedNode final : public ValueAnimatedNode {
 public:
  AdditionAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

 private:
  std::unordered_set<int64_t> m_inputNodes{};
};
} // namespace react::uwp
