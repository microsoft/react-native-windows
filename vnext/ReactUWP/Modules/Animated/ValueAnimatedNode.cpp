#include "pch.h"
#include "ValueAnimatedNode.h"

namespace react {
  namespace uwp {
    ValueAnimatedNode::ValueAnimatedNode(int64_t tag, const folly::dynamic& config) : AnimatedNode(tag)
    {
      m_propertySet = winrt::Window::Current().Compositor().CreatePropertySet();
      m_propertySet.InsertScalar(L"value", static_cast<float>(config.find("value").dereference().second.asDouble()));
      m_propertySet.InsertScalar(L"offset", static_cast<float>(config.find("offset").dereference().second.asDouble()));
    }

    double ValueAnimatedNode::RawValue()
    {
      auto rawValue = 0.0f;
      m_propertySet.TryGetScalar(L"value", rawValue);
      return rawValue;
    }

    void ValueAnimatedNode::RawValue(float value)
    {
      m_propertySet.InsertScalar(L"value", value);
    }

    double ValueAnimatedNode::Offset()
    {
      auto offset = 0.0f;
      m_propertySet.TryGetScalar(L"offset", offset);
      return offset;
    }

    void ValueAnimatedNode::Offset(float offset)
    {
      m_propertySet.InsertScalar(L"offset", offset);
    }

    double ValueAnimatedNode::Value()
    {
      auto rawValue = 0.0f;
      auto offset = 0.0f;
      m_propertySet.TryGetScalar(L"value", rawValue);
      m_propertySet.TryGetScalar(L"offset", offset);
      return rawValue + offset;
    }

    void ValueAnimatedNode::FlattenOffset()
    {
      auto rawValue = 0.0f;
      auto offset = 0.0f;
      m_propertySet.TryGetScalar(L"value", rawValue);
      m_propertySet.TryGetScalar(L"offset", offset);
      m_propertySet.InsertScalar(L"value", rawValue + offset);
      m_propertySet.InsertScalar(L"offset", 0.0f);
    }

    void ValueAnimatedNode::ExtractOffset()
    {
      auto rawValue = 0.0f;
      auto offset = 0.0f;
      m_propertySet.TryGetScalar(L"value", rawValue);
      m_propertySet.TryGetScalar(L"offset", offset);
      m_propertySet.InsertScalar(L"value", 0.0f);
      m_propertySet.InsertScalar(L"offset", rawValue + offset);
    }
  }
}
