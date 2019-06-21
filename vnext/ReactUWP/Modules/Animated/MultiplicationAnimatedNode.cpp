// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MultiplicationAnimatedNode.h"

namespace react { namespace uwp {
  MultiplicationAnimatedNode::MultiplicationAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) : ValueAnimatedNode(tag, config, manager)
  {
    for (auto inputNode : config.find("input").dereference().second)
    {
      m_inputNodes.insert(inputNode.asInt());
    }

    m_propertySet.StartAnimation(L"value",
      [nodes = m_inputNodes, manager]()
    {
      const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();

      anim.Expression([nodes, manager, anim]()
      {
        winrt::hstring expr = L"1";
        for (auto tag : nodes)
        {
          auto identifier = winrt::to_hstring(std::to_string(tag));
          anim.SetReferenceParameter(identifier, manager->m_valueNodes.at(tag)->PropertySet());
          expr = expr + L" * (" + identifier + L".value + " + identifier + L".offset)";
        }
        return expr;
      }());
      return anim;
    }()
      );
  }
} }
