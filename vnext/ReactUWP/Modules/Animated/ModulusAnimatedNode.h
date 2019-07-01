// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ValueAnimatedNode.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  class ModulusAnimatedNode : public ValueAnimatedNode
  {
  public:
    ModulusAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager);

  private:
    int64_t m_inputNodeTag{};
    int64_t m_modulus{};

    static constexpr std::string_view s_modulusName{ "modulus" };

    static constexpr std::wstring_view s_inputParameterName{ L"input" };
    static constexpr std::wstring_view s_modName{ L"mod" };
  };
} }
