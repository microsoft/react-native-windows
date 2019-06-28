// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SubtractionAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace react { namespace uwp {
  SubtractionAnimatedNode::SubtractionAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) : ValueAnimatedNode(tag, config, manager)
  {
    for (const auto& inputNode : config.find(s_inputName).dereference().second)
    {
      if (m_firstInput == s_firstInputUnset)
      {
        m_firstInput = static_cast<int64_t>(inputNode.asDouble());
      }
      else
      {
        m_inputNodes.insert(static_cast<int64_t>(inputNode.asDouble()));
      }
    }

    m_propertySet.StartAnimation(s_valueName,
      [firstNode = m_firstInput, nodes = m_inputNodes, manager]()
      {
        const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();

        anim.Expression([firstNode, nodes, manager, anim]()
        {
          anim.SetReferenceParameter(s_baseName, manager->GetValueAnimatedNode(firstNode).PropertySet());
          winrt::hstring expr = static_cast<winrt::hstring>(L"(") + s_baseName + L"." + s_valueName + L" + " + s_baseName + L"." + s_offsetName + L")";
          for (const auto tag : nodes)
          {
            const auto identifier = std::to_wstring(tag);
            anim.SetReferenceParameter(identifier, manager->GetValueAnimatedNode(tag).PropertySet());
            expr = expr + L" - (" + identifier + L"." + s_valueName + L" + " + identifier + L"." + s_offsetName+ L")";
          }
          return expr;
        }());
        return anim;
      }());
  }
} }
