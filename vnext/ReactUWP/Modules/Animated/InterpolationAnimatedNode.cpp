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
  int arrayIndex = 0;
  for (const auto &rangeValue :
       config.find(s_inputRangeName).dereference().second) {
    m_inputRange[arrayIndex] = rangeValue.asDouble();
    arrayIndex++;
  }

  arrayIndex = 0;
  for (const auto &rangeValue :
       config.find(s_outputRangeName).dereference().second) {
    m_outputRange[arrayIndex] = rangeValue.asDouble();
    arrayIndex++;
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
                static_cast<winrt::hstring>(L"(") + s_parentPropsName + L"." +
                s_offsetName + L" + " + s_parentPropsName + L"." + s_valueName +
                L")") +
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
  animation.SetScalarParameter(
      s_inputMinName, static_cast<float>(m_inputRange[0]));
  animation.SetScalarParameter(
      s_inputMaxName, static_cast<float>(m_inputRange[1]));
  animation.SetScalarParameter(
      s_outputMinName, static_cast<float>(m_outputRange[0]));
  animation.SetScalarParameter(
      s_outputMaxName, static_cast<float>(m_outputRange[1]));
  return animation;
}

winrt::hstring InterpolationAnimatedNode::GetExpression(
    const winrt::hstring &value) {
  return GetLeftExpression(value) + GetRightExpression(value) +
      GetInterpolateExpression(value);
}

winrt::hstring InterpolationAnimatedNode::GetInterpolateExpression(
    const winrt::hstring &value) {
  return s_outputMinName + static_cast<winrt::hstring>(L" + ((") +
      s_outputMaxName + L" - " + s_outputMinName + L") * ((" + value + L" - " +
      s_inputMinName + L") / (" + s_inputMaxName + L" - " + s_inputMinName +
      L")))";
}

winrt::hstring InterpolationAnimatedNode::GetLeftExpression(
    const winrt::hstring &value) {
  switch (ExtrapolationTypeFromString(m_extrapolateLeft)) {
    case ExtrapolationType::Clamp:
      return value + L" < " + s_inputMinName + L" ? " + s_inputMinName + L" : ";
    case ExtrapolationType::Identity:
      return value + L" < " + s_inputMinName + L" ? " + value + L" : ";
    case ExtrapolationType::Extend:
      return value + L" < " + s_inputMinName + L" ? " +
          GetInterpolateExpression(value) + L" : ";
    default:
      return L"";
  }
}

winrt::hstring InterpolationAnimatedNode::GetRightExpression(
    const winrt::hstring &value) {
  switch (ExtrapolationTypeFromString(m_extrapolateRight)) {
    case ExtrapolationType::Clamp:
      return value + L" > " + s_inputMaxName + L" ? " + s_inputMaxName + L" : ";
    case ExtrapolationType::Identity:
      return value + L" > " + s_inputMaxName + L" ? " + value + L" : ";
    case ExtrapolationType::Extend:
      return value + L" > " + s_inputMaxName + L" ? " +
          GetInterpolateExpression(value) + L" : ";
    default:
      return L"";
  }
}
} // namespace uwp
} // namespace react
