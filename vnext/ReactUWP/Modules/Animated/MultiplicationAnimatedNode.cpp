// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MultiplicationAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace react { namespace uwp {
  MultiplicationAnimatedNode::MultiplicationAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) : ValueAnimatedNode(tag, config, manager)
  {
    for (const auto& inputNode : config.find(s_inputName).dereference().second)
    {
      m_inputNodes.insert(static_cast<int64_t>(inputNode.asDouble()));
    }

    m_propertySet.StartAnimation(s_valueName,
      [nodes = m_inputNodes, manager]()
    {
      const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();

      anim.Expression([nodes, manager, anim]()
      {
        winrt::hstring expr = L"1";
        for (const auto tag : nodes)
        {
          auto identifier = std::to_wstring(tag);
          anim.SetReferenceParameter(identifier, manager->GetValueAnimatedNode(tag).PropertySet());
          expr = expr + L" * (" + identifier + L"." + s_valueName + L" + " + identifier + L"." + s_offsetName + L")";
        }
        return expr;
      }());
      return anim;
    }()
      );
  }
} }
