// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "AnimatedNode.h"

namespace Microsoft::ReactNative {
class ColorAnimatedNode final : public AnimatedNode {
 public:
  ColorAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  uint32_t GetColor();

 private:
  void TryApplyNativeColor();

  int32_t m_rNodeId{};
  int32_t m_gNodeId{};
  int32_t m_bNodeId{};
  int32_t m_aNodeId{};
  winrt::Microsoft::ReactNative::JSValue m_nativeColor{};

  static constexpr std::string_view s_rNodeName{"r"};
  static constexpr std::string_view s_gNodeName{"g"};
  static constexpr std::string_view s_bNodeName{"b"};
  static constexpr std::string_view s_aNodeName{"a"};
  static constexpr std::string_view s_nativeColorName{"nativeColor"};
};
} // namespace Microsoft::ReactNative
