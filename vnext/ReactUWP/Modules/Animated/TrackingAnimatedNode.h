// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "AnimatedNode.h"

namespace react {
namespace uwp {
class TrackingAnimatedNode : public AnimatedNode {
 public:
  TrackingAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  void Update() override;

 private:
  void StartAnimation();

  int64_t m_animationId{};
  int64_t m_toValueId{};
  int64_t m_valueId{};
  folly::dynamic m_animationConfig{};

  static constexpr std::string_view s_animationIdName{"animationId"};
  static constexpr std::string_view s_toValueIdName{"toValue"};
  static constexpr std::string_view s_valueIdName{"value"};
  static constexpr std::string_view s_animationConfigName{"animationConfig"};
};
} // namespace uwp
} // namespace react
#pragma once
