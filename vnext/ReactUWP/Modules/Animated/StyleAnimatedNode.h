// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "FacadeType.h"

namespace react {
namespace uwp {
class StyleAnimatedNode : public AnimatedNode {
 public:
  StyleAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void CollectViewUpdates(const folly::dynamic &propsMap);

  std::unordered_map<FacadeType, int64_t> GetMapping();

 private:
  std::map<std::string, int64_t> m_propMapping{};

  static constexpr std::string_view s_styleName{"style"};
};
} // namespace uwp
} // namespace react
