// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ValueAnimatedNode.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {
  enum ExtrapolationType
  {
    identity,
    clamp,
    extend,
  };

  class InterpolationAnimatedNode : public ValueAnimatedNode
  {
  public:
    InterpolationAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager);

    virtual void Update() override;
    virtual void OnDetachedFromNode(int64_t animatedNodeTag) override;
    virtual void OnAttachToNode(int64_t animatedNodeTag) override;

    static constexpr std::wstring_view ExtrapolateTypeIdentity = L"identity";
    static constexpr std::wstring_view ExtrapolateTypeClamp = L"clamp";
    static constexpr std::wstring_view ExtrapolateTypeExtend = L"extend";

  private:
    static ExtrapolationType ExtrapolationTypeFromString(const std::string& type);
    winrt::ExpressionAnimation createExpressionAnimation(const winrt::Compositor& compositor, const std::shared_ptr<ValueAnimatedNode>& parent);

    winrt::hstring GetExpression(const winrt::hstring& value);
    winrt::hstring GetInterpolateExpression(const winrt::hstring& value);
    winrt::hstring GetLeftExpression(const winrt::hstring& value);
    winrt::hstring GetRightExpression(const winrt::hstring& value);
    winrt::ExpressionAnimation m_rawValueAnimation{ nullptr };
    winrt::ExpressionAnimation m_offsetAnimation{ nullptr };
    std::array<double, 2> m_inputRange;
    std::array<double, 2> m_outputRange;
    std::string m_extrapolateLeft;
    std::string m_extrapolateRight;

    int64_t m_parentTag{ -1 };
  };
} }
