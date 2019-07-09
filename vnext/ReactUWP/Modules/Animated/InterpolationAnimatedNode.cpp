// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ExtrapolationType.h"
#include "InterpolationAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace react {
namespace uwp {
InterpolationAnimatedNode::InterpolationAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, manager) {
  for (const auto &rangeValue :
       config.find(s_inputRangeName).dereference().second) {
    m_inputRanges.push_back(rangeValue.asDouble());
  }
  for (const auto &rangeValue :
       config.find(s_outputRangeName).dereference().second) {
    m_outputRanges.push_back(rangeValue.asDouble());
  }

  m_extrapolateLeft =
      config.find(s_extrapolateLeftName).dereference().second.asString();
  m_extrapolateRight =
      config.find(s_extrapolateRightName).dereference().second.asString();
}

void InterpolationAnimatedNode::Update() {}

void InterpolationAnimatedNode::OnDetachedFromNode(int64_t animatedNodeTag) {
  assert(m_parentTag == animatedNodeTag);
  m_parentTag = s_parentTagUnset;
  m_propertySet.StopAnimation(s_valueName);
  m_propertySet.StopAnimation(s_valueName);
  m_rawValueAnimation = nullptr;
  m_offsetAnimation = nullptr;
}

void InterpolationAnimatedNode::OnAttachToNode(int64_t animatedNodeTag) {
  assert(m_parentTag == s_parentTagUnset);
  m_parentTag = animatedNodeTag;

  const auto [rawValueAnimation, offsetAnimation] = [this]() {
    if (const auto manager = m_manager.lock()) {
      if (const auto parent = manager->GetValueAnimatedNode(m_parentTag)) {
        const auto compositor = winrt::Window::Current().Compositor();

        const auto rawValueAnimation =
            CreateExpressionAnimation(compositor, *parent);
        rawValueAnimation.Expression(GetExpression(
            s_parentPropsName + static_cast<winrt::hstring>(L".") +
            s_valueName));

        const auto offsetAnimation =
            CreateExpressionAnimation(compositor, *parent);
        offsetAnimation.Expression(
            L"(" +
            GetExpression(
                s_parentPropsName + static_cast<winrt::hstring>(L".") +
                s_offsetName + L" + " + s_parentPropsName + L"." +
                s_valueName) +
            L") - this.target." + s_valueName);

        return std::make_tuple(rawValueAnimation, offsetAnimation);
      }
    }
    return std::tuple<winrt::ExpressionAnimation, winrt::ExpressionAnimation>(
        nullptr, nullptr);
  }();

  m_propertySet.StartAnimation(s_valueName, rawValueAnimation);
  m_propertySet.StartAnimation(s_offsetName, offsetAnimation);

  m_rawValueAnimation = rawValueAnimation;
  m_offsetAnimation = offsetAnimation;
}

winrt::ExpressionAnimation InterpolationAnimatedNode::CreateExpressionAnimation(
    const winrt::Compositor &compositor,
    ValueAnimatedNode &parent) {
  const auto animation = compositor.CreateExpressionAnimation();
  animation.SetReferenceParameter(s_parentPropsName, parent.PropertySet());
  for (size_t i = 0; i < m_inputRanges.size(); i++) {
    animation.SetScalarParameter(
        s_inputName.data() + std::to_wstring(i),
        static_cast<float>(m_inputRanges[i]));
  }
  for (size_t i = 0; i < m_outputRanges.size(); i++) {
    animation.SetScalarParameter(
        s_outputName.data() + std::to_wstring(i),
        static_cast<float>(m_outputRanges[i]));
  }
  return animation;
}

winrt::hstring InterpolationAnimatedNode::GetExpression(
    const winrt::hstring &value) {
  const auto leftInterpolateExpression = GetInterpolateExpression(
      value,
      s_inputName.data() + std::to_wstring(0),
      s_inputName.data() + std::to_wstring(1),
      s_outputName.data() + std::to_wstring(0),
      s_outputName.data() + std::to_wstring(1));

  const auto size = m_inputRanges.size();
  const auto rightInterpolateExpression = GetInterpolateExpression(
      value,
      s_inputName.data() + std::to_wstring(size - 2),
      s_inputName.data() + std::to_wstring(size - 1),
      s_outputName.data() + std::to_wstring(size - 2),
      s_outputName.data() + std::to_wstring(size - 1));

  auto returnValue = GetLeftExpression(value, leftInterpolateExpression) +
      GetRightExpression(value, rightInterpolateExpression);

  // Start at 1 because we use the index and previous for each step.
  for (size_t i = 1; i < size - 1; i++) {
    const std::wstring inMin = s_inputName.data() + std::to_wstring(i - 1);
    const std::wstring inMax = s_inputName.data() + std::to_wstring(i);
    const std::wstring outMin = s_outputName.data() + std::to_wstring(i - 1);
    const std::wstring outMax = s_outputName.data() + std::to_wstring(i);

    returnValue = returnValue + value + L" >= " + inMin + L" && " + value +
        L" <= " + inMax + L" ? " +
        GetInterpolateExpression(value, inMin, inMax, outMin, outMax) + L" : ";
  }

  returnValue = returnValue + // L"asd" +
      GetInterpolateExpression(
                    value,
                    s_inputName.data() + std::to_wstring(size - 2),
                    s_inputName.data() + std::to_wstring(size - 1),
                    s_outputName.data() + std::to_wstring(size - 2),
                    s_outputName.data() + std::to_wstring(size - 1));

  return returnValue;
}

winrt::hstring InterpolationAnimatedNode::GetInterpolateExpression(
    const winrt::hstring &value,
    const std::wstring &inputMin,
    const std::wstring &inputMax,
    const std::wstring &outputMin,
    const std::wstring &outputMax) {
  return outputMin + static_cast<winrt::hstring>(L" + ((") + outputMax +
      L" - " + outputMin + L") * ((" + value + L" - " + inputMin + L") / (" +
      inputMax + L" - " + inputMin + L")))";
}

winrt::hstring InterpolationAnimatedNode::GetLeftExpression(
    const winrt::hstring &value,
    const winrt::hstring &leftInterpolateExpression) {
  const auto firstInput = s_inputName.data() + std::to_wstring(0);
  switch (ExtrapolationTypeFromString(m_extrapolateLeft)) {
    case ExtrapolationType::Clamp:
      return value + L" < " + firstInput + L" ? " + firstInput + L" : ";
    case ExtrapolationType::Identity:
      return value + L" < " + firstInput + L" ? " + value + L" : ";
    case ExtrapolationType::Extend:
      return value + L" < " + firstInput + L" ? " + leftInterpolateExpression +
          L" : ";
    default:
      return L"";
  }
}

winrt::hstring InterpolationAnimatedNode::GetRightExpression(
    const winrt::hstring &value,
    const winrt::hstring &rightInterpolateExpression) {
  const auto lastInput =
      s_inputName.data() + std::to_wstring(m_inputRanges.size() - 1);
  switch (ExtrapolationTypeFromString(m_extrapolateRight)) {
    case ExtrapolationType::Clamp:
      return value + L" > " + lastInput + L" ? " + lastInput + L" : ";
    case ExtrapolationType::Identity:
      return value + L" > " + lastInput + L" ? " + value + L" : ";
    case ExtrapolationType::Extend:
      return value + L" > " + lastInput + L" ? " + rightInterpolateExpression +
          L" : ";
    default:
      return L"";
  }
}

} // namespace uwp
} // namespace react
