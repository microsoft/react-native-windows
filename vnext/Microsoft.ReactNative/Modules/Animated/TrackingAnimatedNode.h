// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <JSValue.h>
#include "AnimatedNode.h"

namespace Microsoft::ReactNative {
class TrackingAnimatedNode final : public AnimatedNode {
 public:
  TrackingAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  virtual void Update() override;
  void StartAnimation();

 private:
  int64_t m_animationId{};
  int64_t m_toValueId{};
  int64_t m_valueId{};
  winrt::Microsoft::ReactNative::JSValueObject m_animationConfig;

  static constexpr std::string_view s_animationIdName{"animationId"};
  static constexpr std::string_view s_toValueIdName{"toValue"};
  static constexpr std::string_view s_valueIdName{"value"};
  static constexpr std::string_view s_animationConfigName{"animationConfig"};
};
} // namespace Microsoft::ReactNative
#pragma once
