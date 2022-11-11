// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "ValueAnimatedNode.h"

namespace Microsoft::ReactNative {
class DiffClampAnimatedNode final : public ValueAnimatedNode {
 public:
  DiffClampAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  virtual void Update() override;

 private:
  int64_t m_inputNodeTag{};
  double m_min{};
  double m_max{};
  double m_lastValue{};

  static constexpr std::string_view s_minName{"min"};
  static constexpr std::string_view s_maxName{"max"};

  static constexpr std::wstring_view s_inputParameterName{L"input"};
  static constexpr std::wstring_view s_minParameterName{L"min"};
  static constexpr std::wstring_view s_maxParameterName{L"max"};
};
} // namespace Microsoft::ReactNative
