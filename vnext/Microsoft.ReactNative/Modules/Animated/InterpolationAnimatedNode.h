// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "ValueAnimatedNode.h"

namespace Microsoft::ReactNative {

class InterpolationAnimatedNode final : public ValueAnimatedNode {
 public:
  InterpolationAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

  virtual void Update() override;
  virtual void OnDetachedFromNode(int64_t animatedNodeTag) override;
  virtual void OnAttachToNode(int64_t animatedNodeTag) override;

  static constexpr std::string_view ExtrapolateTypeIdentity = "identity";
  static constexpr std::string_view ExtrapolateTypeClamp = "clamp";
  static constexpr std::string_view ExtrapolateTypeExtend = "extend";

 private:
  comp::ExpressionAnimation CreateExpressionAnimation(const winrt::Compositor &compositor, ValueAnimatedNode &parent);

  winrt::hstring GetExpression(const winrt::hstring &value);
  winrt::hstring GetInterpolateExpression(
      const winrt::hstring &value,
      const std::wstring &inputMin,
      const std::wstring &inputMax,
      const std::wstring &outputMin,
      const std::wstring &outputMax);
  winrt::hstring GetLeftExpression(const winrt::hstring &value, const winrt::hstring &leftInterpolateExpression);
  winrt::hstring GetRightExpression(const winrt::hstring &, const winrt::hstring &rightInterpolateExpression);

  double InterpolateValue(double value);

  comp::ExpressionAnimation m_rawValueAnimation{nullptr};
  comp::ExpressionAnimation m_offsetAnimation{nullptr};
  std::vector<double> m_inputRanges;
  std::vector<double> m_outputRanges;
  std::string m_extrapolateLeft;
  std::string m_extrapolateRight;

  int64_t m_parentTag{s_parentTagUnset};

  static constexpr int64_t s_parentTagUnset{-1};

  static constexpr std::string_view s_inputRangeName{"inputRange"};
  static constexpr std::string_view s_outputRangeName{"outputRange"};
  static constexpr std::string_view s_extrapolateLeftName{"extrapolateLeft"};
  static constexpr std::string_view s_extrapolateRightName{"extrapolateRight"};

  static constexpr std::wstring_view s_parentPropsName{L"p"};
  static constexpr std::wstring_view s_inputName{L"i"};
  static constexpr std::wstring_view s_outputName{L"o"};
};
} // namespace Microsoft::ReactNative
