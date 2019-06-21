// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ModulusAnimatedNode.h"

namespace react { namespace uwp {
  ModulusAnimatedNode::ModulusAnimatedNode(int64_t tag, const folly::dynamic& config, const std::shared_ptr<NativeAnimatedNodeManager>& manager) : ValueAnimatedNode(tag, config, manager)
  {
    m_inputNodeTag = config.find("input").dereference().second.asInt();
    m_modulus = config.find("modulus").dereference().second.asInt();

    m_propertySet.StartAnimation(L"value",
      [node = m_inputNodeTag, mod = m_modulus, manager]()
      {
        const auto anim = winrt::Window::Current().Compositor().CreateExpressionAnimation();
        anim.SetReferenceParameter(L"input", manager->m_valueNodes.at(node)->PropertySet());
        anim.SetScalarParameter(L"mod", static_cast<float>(mod));
        anim.Expression(L"(input.value + input.offset) % mod");
        return anim;
      }()
    );
  }
} }
