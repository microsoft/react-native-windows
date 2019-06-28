// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ValueAnimatedNode.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  class SubtractionAnimatedNode : public ValueAnimatedNode
  {
  public:
    SubtractionAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager);

  private:
    int64_t m_firstInput{ s_firstInputUnset };
    std::unordered_set<int64_t> m_inputNodes{};

    static constexpr int64_t s_firstInputUnset{ -1 };

    static constexpr std::wstring_view s_baseName{ L"base" };
  };
} }
