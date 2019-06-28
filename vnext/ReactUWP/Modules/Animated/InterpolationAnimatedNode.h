// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ValueAnimatedNode.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {

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
    winrt::ExpressionAnimation CreateExpressionAnimation(const winrt::Compositor& compositor, ValueAnimatedNode& parent);

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

    int64_t m_parentTag{ s_parentTagUnset };

    static constexpr int64_t s_parentTagUnset{ -1 };

    static constexpr std::string_view s_inputRangeName{ "inputRange" };
    static constexpr std::string_view s_outputRangeName{ "outputRange" };
    static constexpr std::string_view s_extrapolateLeftName{ "extrapolateLeft" };
    static constexpr std::string_view s_extrapolateRightName{ "extrapolateRight" };

    static constexpr std::wstring_view s_parentPropsName{ L"parentProps" };
    static constexpr std::wstring_view s_inputMinName{ L"inputMin" };
    static constexpr std::wstring_view s_inputMaxName{ L"inputMax" };
    static constexpr std::wstring_view s_outputMinName{ L"outputMin" };
    static constexpr std::wstring_view s_outputMaxName{ L"outputMax" };
  };
} }
