// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SubtractionAnimatedNode.h"

namespace react { namespace uwp {
  SubtractionAnimatedNode::SubtractionAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) : ValueAnimatedNode(tag, config, manager)
  {
    for (auto inputNode : config.find(s_inputName).dereference().second)
    {
      if (m_firstInput == s_firstInputUnset)
      {
        m_firstInput = inputNode.asInt();
      }
      else
      {
        m_inputNodes.insert(inputNode.asInt());
      }
    }

    m_propertySet.StartAnimation(s_valueName,
      [firstNode = m_firstInput, nodes = m_inputNodes, manager]()
      {
        const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();

        anim.Expression([firstNode, nodes, manager, anim]()
        {
          anim.SetReferenceParameter(s_baseName, manager->m_valueNodes.at(firstNode)->PropertySet());
          winrt::hstring expr = static_cast<winrt::hstring>(L"(") + s_baseName + L"." + s_valueName + L" + " + s_baseName + L"." + s_offsetName + L")";
          for (auto tag : nodes)
          {
            auto identifier = winrt::to_hstring(std::to_string(tag));
            anim.SetReferenceParameter(identifier, manager->m_valueNodes.at(tag)->PropertySet());
            expr = expr + L" - (" + identifier + L"." + s_valueName + L" + " + identifier + L"." + s_offsetName+ L")";
          }
          return expr;
        }());
        return anim;
      }());
  }
} }
