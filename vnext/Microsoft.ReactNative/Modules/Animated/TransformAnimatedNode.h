// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <JSValue.h>
#include "AnimatedNode.h"
#include "FacadeType.h"
#include "JSValue.h"

namespace Microsoft::ReactNative {
struct TransformConfig {
 public:
  std::string property;
  int64_t nodeTag;
  double value;
};

class TransformAnimatedNode final : public AnimatedNode {
 public:
  TransformAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  std::unordered_map<FacadeType, int64_t> GetMapping();
  void CollectViewUpdates(winrt::Microsoft::ReactNative::JSValueObject &props);

 private:
  std::vector<TransformConfig> m_transformConfigs;

  static constexpr int64_t s_unsetNodeTag{-1};

  static constexpr std::string_view s_transformsName{"transforms"};
  static constexpr std::string_view s_propertyName{"property"};
  static constexpr std::string_view s_typeName{"type"};
  static constexpr std::string_view s_animatedName{"animated"};
  static constexpr std::string_view s_nodeTagName{"nodeTag"};
  static constexpr std::string_view s_valueName{"value"};
  static constexpr std::string_view s_transformPropName{"transform"};
};
} // namespace Microsoft::ReactNative
