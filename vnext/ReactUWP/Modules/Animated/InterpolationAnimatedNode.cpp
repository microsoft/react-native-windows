#include "pch.h"
#include "InterpolationAnimatedNode.h"

namespace react {
  namespace uwp {
    InterpolationAnimatedNode::InterpolationAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodesManager>& manager) : ValueAnimatedNode(tag, manager)
    {
      int arrayIndex = 0;
      for (auto rangeValue : config.find("inputRange").dereference().second)
      {
        m_inputRange[arrayIndex] = rangeValue.asDouble();
        arrayIndex++;
      }

      arrayIndex = 0;
      for (auto rangeValue : config.find("outputRange").dereference().second)
      {
        m_outputRange[arrayIndex] = rangeValue.asDouble();
        arrayIndex++;
      }

      m_extrapolateLeft = config.find("extrapolateLeft").dereference().second.asString();
      m_extrapolateRight = config.find("extrapolateRight").dereference().second.asString();
    }

    void InterpolationAnimatedNode::Update()
    {

    }

    void InterpolationAnimatedNode::OnDetachedFromNode(int64_t animatedNodeTag)
    {
      assert(m_parentTag == animatedNodeTag);
      m_parentTag = -1;
      m_propertySet.StopAnimation(L"value");
      m_propertySet.StopAnimation(L"offset");
      m_rawValueAnimation = nullptr;
      m_offsetAnimation = nullptr;
    }

    void InterpolationAnimatedNode::OnAttachToNode(int64_t animatedNodeTag)
    {
      assert(m_parentTag == -1);
      m_parentTag = animatedNodeTag;

      const auto [rawValueAnimation, offsetAnimation] = [this]()
      {
        if (auto manager = m_manager.lock())
        {
          if (auto parent = manager->m_valueNodes.at(m_parentTag))
          {
            auto compositor = winrt::Window::Current().Compositor();

            auto rawValueAnimation = createExpressionAnimation(compositor, parent);
            rawValueAnimation.Expression(GetExpression(L"parentProps.value"));

            auto offsetAnimation = createExpressionAnimation(compositor, parent);
            offsetAnimation.Expression(L"(" + GetExpression(L"(parentProps.offset + parentProps.value)") + L") - this.target.value");

            return std::make_tuple(rawValueAnimation, offsetAnimation);
          }
        }
        return std::tuple<winrt::ExpressionAnimation, winrt::ExpressionAnimation>(nullptr, nullptr);
      }();

      m_propertySet.StartAnimation(L"value", rawValueAnimation);
      m_propertySet.StartAnimation(L"offset", offsetAnimation);

      m_rawValueAnimation = rawValueAnimation;
      m_offsetAnimation = offsetAnimation;
    }

    winrt::ExpressionAnimation InterpolationAnimatedNode::createExpressionAnimation(const winrt::Compositor& compositor, const std::shared_ptr<ValueAnimatedNode>& parent)
    {
      auto animation = compositor.CreateExpressionAnimation();
      animation.SetReferenceParameter(L"parentProps", parent->PropertySet());
      animation.SetScalarParameter(L"inputMin", static_cast<float>(m_inputRange[0]));
      animation.SetScalarParameter(L"inputMax", static_cast<float>(m_inputRange[1]));
      animation.SetScalarParameter(L"outputMin", static_cast<float>(m_outputRange[0]));
      animation.SetScalarParameter(L"outputMax", static_cast<float>(m_outputRange[1]));
      return animation;
    }

    winrt::hstring InterpolationAnimatedNode::GetExpression(const winrt::hstring& value)
    {
      return GetLeftExpression(value) + GetRightExpression(value) + GetInterpolateExpression(value);
    }

    winrt::hstring InterpolationAnimatedNode::GetInterpolateExpression(const winrt::hstring& value)
    {
      return L"outputMin + ((outputMax - outputMin) * ((" + value + L" - inputMin) / (inputMax - inputMin)))";
    }

    winrt::hstring InterpolationAnimatedNode::GetLeftExpression(const winrt::hstring& value)
    {
      switch (ExtrapolationTypeFromString(m_extrapolateLeft))
      {
      case ExtrapolationType::clamp:
        return value + L" < inputMin ? inputMin : ";
      case ExtrapolationType::identity:
        return value + L" < inputMin ? " + value + L" : ";
      case ExtrapolationType::extend:
        return value + L" < inputMin ? " + GetInterpolateExpression(value) + L" : ";
      default:
        return L"";

      }
    }

    winrt::hstring InterpolationAnimatedNode::GetRightExpression(const winrt::hstring& value)
    {
      switch (ExtrapolationTypeFromString(m_extrapolateRight))
      {
      case ExtrapolationType::clamp:
        return value + L" > inputMax ? inputMax : ";
      case ExtrapolationType::identity:
        return value + L" > inputMax ? " + value + L" : ";
      case ExtrapolationType::extend:
        return value + L" > inputMax ? " + GetInterpolateExpression(value) + L" : ";
      default:
        return L"";

      }
    }

    ExtrapolationType InterpolationAnimatedNode::ExtrapolationTypeFromString(const std::string& string)
    {
      if (string == "identity") return ExtrapolationType::identity;
      else if (string == "clamp") return ExtrapolationType::clamp;
      else
      {
        assert(string == "extend");
        return ExtrapolationType::extend;
      }
    }
  }
}
