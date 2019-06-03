#pragma once
#include "pch.h"
#include "AnimatedNode.h"
#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    class ValueAnimatedNode : public AnimatedNode
    {
    public:
      ValueAnimatedNode(int64_t tag, const folly::dynamic& config);
      double Value();
      double RawValue();
      void RawValue(float value);
      double Offset();
      void Offset(float offset);
      void FlattenOffset();
      void ExtractOffset();
      winrt::CompositionPropertySet PropertySet() { return m_propertySet; };

    private:
      winrt::CompositionPropertySet m_propertySet{ nullptr };

    };
  }
}
