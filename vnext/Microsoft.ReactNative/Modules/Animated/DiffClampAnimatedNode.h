// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "ValueAnimatedNode.h"

namespace react::uwp {
class DiffClampAnimatedNode final : public ValueAnimatedNode {
 public:
  DiffClampAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

 private:
  int64_t m_inputNodeTag{};
  double m_min{};
  double m_max{};

  static constexpr std::string_view s_minName{"min"};
  static constexpr std::string_view s_maxName{"max"};

  static constexpr std::wstring_view s_inputParameterName{L"input"};
  static constexpr std::wstring_view s_minParameterName{L"min"};
  static constexpr std::wstring_view s_maxParameterName{L"max"};
};
} // namespace react::uwp
