// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "FacadeType.h"

namespace react {
namespace uwp {
struct TransformConfig {
 public:
  std::string property;
  int64_t nodeTag;
  double value;
};

class TransformAnimatedNode : public AnimatedNode {
 public:
  TransformAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  std::unordered_map<FacadeType, int64_t> GetMapping();

 private:
  std::vector<TransformConfig> m_transformConfigs;

  static constexpr int64_t s_unsetNodeTag{-1};

  static constexpr std::string_view s_transformsName{"transforms"};
  static constexpr std::string_view s_propertyName{"property"};
  static constexpr std::string_view s_typeName{"type"};
  static constexpr std::string_view s_animatedName{"animated"};
  static constexpr std::string_view s_nodeTagName{"nodeTag"};
  static constexpr std::string_view s_valueName{"value"};
};
} // namespace uwp
} // namespace react
