// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include "ValueAnimatedNode.h"

namespace Microsoft::ReactNative {
class ModulusAnimatedNode final : public ValueAnimatedNode {
 public:
  ModulusAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

 private:
  int64_t m_inputNodeTag{};
  int64_t m_modulus{};

  static constexpr std::string_view s_modulusName{"modulus"};

  static constexpr std::wstring_view s_inputParameterName{L"input"};
  static constexpr std::wstring_view s_modName{L"mod"};
};
} // namespace Microsoft::ReactNative
