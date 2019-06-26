// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AdditionAnimatedNode.h"

namespace react {
  namespace uwp {
    AdditionAnimatedNode::AdditionAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) : ValueAnimatedNode(tag, config, manager)
    {
      for (auto inputNode : config.find(s_inputName).dereference().second)
      {
        m_inputNodes.insert(inputNode.asInt());
      }

      m_propertySet.StartAnimation(s_valueName,
        [nodes = m_inputNodes, manager]()
        {
          const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();

          anim.Expression([nodes, manager, anim]()
          {
            winrt::hstring expr = L"0";
            for (auto tag : nodes)
            {
              auto identifier = winrt::to_hstring(std::to_string(tag));
              anim.SetReferenceParameter(identifier, manager->m_valueNodes.at(tag)->PropertySet());
              expr = expr + L" + " + identifier + L"." + s_valueName + L" + " + identifier + L"." + s_offsetName;
            }
            return expr;
          }());
          return anim;
        }()
      );
    }
  }
}
