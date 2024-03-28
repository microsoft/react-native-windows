// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AnimationUtils.h"
#include "ExtrapolationType.h"
#include "InterpolationAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"
#include "Utils/ValueUtils.h"

namespace Microsoft::ReactNative {

inline int32_t ColorToInt(winrt::Windows::UI::Color color) {
  return static_cast<uint8_t>(color.A) << 24 | static_cast<uint8_t>(color.R) << 16 |
      static_cast<uint8_t>(color.G) << 8 | static_cast<uint8_t>(color.B);
}

inline uint8_t ScaleByte(uint8_t min, uint8_t max, double ratio) {
  const auto scaledValue = min + (max - min) * ratio;
  const auto clampedValue = std::clamp(static_cast<uint32_t>(std::round(scaledValue)), 0u, 255u);
  return static_cast<uint8_t>(clampedValue);
}

InterpolationAnimatedNode::InterpolationAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, config, manager) {
  for (const auto &rangeValue : config[s_inputRangeName].AsArray()) {
    m_inputRanges.push_back(rangeValue.AsDouble());
  }

  const auto isColorOutput = config[s_outputTypeName].AsString() == s_colorOutputType;
  if (!m_useComposition && isColorOutput) {
    m_isColorOutput = true;
    for (const auto &rangeValue : config[s_outputRangeName].AsArray()) {
      m_colorOutputRanges.push_back(ColorFrom(rangeValue));
    }
  } else {
    assert(!isColorOutput && "Color interpolation not supported");
    for (const auto &rangeValue : config[s_outputRangeName].AsArray()) {
      m_defaultOutputRanges.push_back(rangeValue.AsDouble());
    }
  }

  m_extrapolateLeft = config[s_extrapolateLeftName].AsString();
  m_extrapolateRight = config[s_extrapolateRightName].AsString();
}

void InterpolationAnimatedNode::Update() {
  assert(!m_useComposition);
  if (m_parentTag == s_parentTagUnset) {
    return;
  }

  if (const auto manager = m_manager.lock()) {
    if (const auto node = manager->GetValueAnimatedNode(m_parentTag)) {
      if (m_isColorOutput) {
        RawValue(InterpolateColor(node->Value()));
      } else {
        RawValue(InterpolateValue(node->Value()));
      }
    }
  }
}

void InterpolationAnimatedNode::OnDetachedFromNode([[maybe_unused]] int64_t animatedNodeTag) {
  assert(m_parentTag == animatedNodeTag);
  m_parentTag = s_parentTagUnset;

  if (m_useComposition) {
    m_propertySet.StopAnimation(s_valueName);
    m_propertySet.StopAnimation(s_offsetName);
    m_rawValueAnimation = nullptr;
    m_offsetAnimation = nullptr;
  }
}

void InterpolationAnimatedNode::OnAttachToNode(int64_t animatedNodeTag) {
  assert(HasCompatibleAnimationDriver(animatedNodeTag));
  assert(m_parentTag == s_parentTagUnset);
  m_parentTag = animatedNodeTag;

  if (m_useComposition) {
    const auto [rawValueAnimation, offsetAnimation] = [this]() {
      if (const auto manager = m_manager.lock()) {
        if (const auto parent = manager->GetValueAnimatedNode(m_parentTag)) {
          const auto compositor = manager->Compositor();

          const auto rawValueAnimation = CreateExpressionAnimation(compositor, *parent);
          rawValueAnimation.Expression(
              GetExpression(s_parentPropsName + static_cast<winrt::hstring>(L".") + s_valueName));

          const auto offsetAnimation = CreateExpressionAnimation(compositor, *parent);
          offsetAnimation.Expression(
              L"(" +
              GetExpression(
                  s_parentPropsName + static_cast<winrt::hstring>(L".") + s_offsetName + L" + " + s_parentPropsName +
                  L"." + s_valueName) +
              L") - this.target." + s_valueName);

          return std::make_tuple(rawValueAnimation, offsetAnimation);
        }
      }
      return std::tuple<comp::ExpressionAnimation, comp::ExpressionAnimation>(nullptr, nullptr);
    }();

    m_propertySet.StartAnimation(s_valueName, rawValueAnimation);
    m_propertySet.StartAnimation(s_offsetName, offsetAnimation);

    m_rawValueAnimation = rawValueAnimation;
    m_offsetAnimation = offsetAnimation;
  }
}

comp::ExpressionAnimation InterpolationAnimatedNode::CreateExpressionAnimation(
    const winrt::Compositor &compositor,
    ValueAnimatedNode &parent) {
  const auto animation = compositor.CreateExpressionAnimation();
  animation.SetReferenceParameter(s_parentPropsName, parent.PropertySet());
  for (size_t i = 0; i < m_inputRanges.size(); i++) {
    animation.SetScalarParameter(s_inputName.data() + std::to_wstring(i), static_cast<float>(m_inputRanges[i]));
  }
  for (size_t i = 0; i < m_defaultOutputRanges.size(); i++) {
    animation.SetScalarParameter(
        s_outputName.data() + std::to_wstring(i), static_cast<float>(m_defaultOutputRanges[i]));
  }
  return animation;
}

winrt::hstring InterpolationAnimatedNode::GetExpression(const winrt::hstring &value) {
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

  auto returnValue =
      GetLeftExpression(value, leftInterpolateExpression) + GetRightExpression(value, rightInterpolateExpression);

  // Start at 1 because we use the index and previous for each step.
  for (size_t i = 1; i < size - 1; i++) {
    const std::wstring inMin = s_inputName.data() + std::to_wstring(i - 1);
    const std::wstring inMax = s_inputName.data() + std::to_wstring(i);
    const std::wstring outMin = s_outputName.data() + std::to_wstring(i - 1);
    const std::wstring outMax = s_outputName.data() + std::to_wstring(i);

    returnValue = returnValue + value + L" >= " + inMin + L" && " + value + L" <= " + inMax + L" ? " +
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
  return outputMin + static_cast<winrt::hstring>(L" + ((") + outputMax + L" - " + outputMin + L") * ((" + value +
      L" - " + inputMin + L") / (" + inputMax + L" - " + inputMin + L")))";
}

winrt::hstring InterpolationAnimatedNode::GetLeftExpression(
    const winrt::hstring &value,
    const winrt::hstring &leftInterpolateExpression) {
  const auto firstInput = s_inputName.data() + std::to_wstring(0);
  const auto firstOutput = s_outputName.data() + std::to_wstring(0);
  switch (ExtrapolationTypeFromString(m_extrapolateLeft)) {
    case ExtrapolationType::Clamp:
      return value + L" < " + firstInput + L" ? " + firstOutput + L" : ";
    case ExtrapolationType::Identity:
      return value + L" < " + firstInput + L" ? " + value + L" : ";
    case ExtrapolationType::Extend:
      return value + L" < " + firstInput + L" ? " + leftInterpolateExpression + L" : ";
    default:
      return L"";
  }
}

winrt::hstring InterpolationAnimatedNode::GetRightExpression(
    const winrt::hstring &value,
    const winrt::hstring &rightInterpolateExpression) {
  const auto lastInput = s_inputName.data() + std::to_wstring(m_inputRanges.size() - 1);
  const auto lastOutput = s_outputName.data() + std::to_wstring(m_defaultOutputRanges.size() - 1);
  switch (ExtrapolationTypeFromString(m_extrapolateRight)) {
    case ExtrapolationType::Clamp:
      return value + L" > " + lastInput + L" ? " + lastOutput + L" : ";
    case ExtrapolationType::Identity:
      return value + L" > " + lastInput + L" ? " + value + L" : ";
    case ExtrapolationType::Extend:
      return value + L" > " + lastInput + L" ? " + rightInterpolateExpression + L" : ";
    default:
      return L"";
  }
}

double InterpolationAnimatedNode::InterpolateValue(double value) {
  // Compute range index
  size_t index = 1;
  for (; index < m_inputRanges.size() - 1; ++index) {
    if (m_inputRanges[index] >= value) {
      break;
    }
  }
  index--;

  return Interpolate(
      value,
      m_inputRanges[index],
      m_inputRanges[index + 1],
      m_defaultOutputRanges[index],
      m_defaultOutputRanges[index + 1],
      m_extrapolateLeft,
      m_extrapolateRight);
}

double InterpolationAnimatedNode::InterpolateColor(double value) {
  // Compute range index
  size_t index = 1;
  for (; index < m_inputRanges.size() - 1; ++index) {
    if (m_inputRanges[index] >= value) {
      break;
    }
  }
  index--;

  double result;
  const auto outputMin = m_colorOutputRanges[index];
  const auto outputMax = m_colorOutputRanges[index + 1];
  const auto outputMinInt = ColorToInt(outputMin);
  const auto outputMaxInt = ColorToInt(outputMax);
  if (outputMin == outputMax) {
    memcpy(&result, &outputMinInt, sizeof(int32_t));
    return result;
  }

  const auto inputMin = m_inputRanges[index];
  const auto inputMax = m_inputRanges[index + 1];
  if (inputMin == inputMax) {
    if (value <= inputMin) {
      memcpy(&result, &outputMinInt, sizeof(int32_t));
    } else {
      memcpy(&result, &outputMaxInt, sizeof(int32_t));
    }
    return result;
  }

  const auto ratio = (value - inputMin) / (inputMax - inputMin);
  const auto interpolatedColor = ScaleByte(outputMin.A, outputMax.A, ratio) << 24 |
      ScaleByte(outputMin.R, outputMax.R, ratio) << 16 | ScaleByte(outputMin.G, outputMax.G, ratio) << 8 |
      ScaleByte(outputMin.B, outputMax.B, ratio);
  memcpy(&result, &interpolatedColor, sizeof(int32_t));
  return result;
}

} // namespace Microsoft::ReactNative
