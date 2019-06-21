// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DivisionAnimatedNode.h"

namespace react { namespace uwp {
  DivisionAnimatedNode::DivisionAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) : ValueAnimatedNode(tag, config, manager)
  {
    for (auto inputNode : config.find("input").dereference().second)
    {
      if (m_firstInput == -1)
      {
        m_firstInput = inputNode.asInt();
      }
      else
      {
        m_inputNodes.insert(inputNode.asInt());
      }
    }

    m_propertySet.StartAnimation(L"value",
      [firstNode = m_firstInput, nodes = m_inputNodes, manager]()
      {
        const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();

        anim.Expression([firstNode, nodes, manager, anim]()
        {
          anim.SetReferenceParameter(L"base", manager->m_valueNodes.at(firstNode)->PropertySet());
          winrt::hstring expr = L"(base.value + base.offset)";
          for (auto tag : nodes)
          {
            auto identifier = winrt::to_hstring(std::to_string(tag));
            anim.SetReferenceParameter(identifier, manager->m_valueNodes.at(tag)->PropertySet());
            expr = expr + L" / (" + identifier + L".value + " + identifier + L".offset)";
          }
          return expr;
        }());
        return anim;
      }()
    );
  }
} }
