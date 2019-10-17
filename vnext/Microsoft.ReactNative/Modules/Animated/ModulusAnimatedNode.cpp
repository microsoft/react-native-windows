// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ModulusAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace react {
namespace uwp {
ModulusAnimatedNode::ModulusAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, config, manager) {
  m_inputNodeTag = static_cast<int64_t>(
      config.find(s_inputName).dereference().second.asDouble());
  m_modulus = static_cast<int64_t>(
      config.find(s_modulusName).dereference().second.asDouble());

  m_propertySet.StartAnimation(
      s_valueName, [node = m_inputNodeTag, mod = m_modulus, manager]() {
        const auto anim =
            winrt::Window::Current().Compositor().CreateExpressionAnimation();
        anim.SetReferenceParameter(
            s_inputParameterName,
            manager->GetValueAnimatedNode(node)->PropertySet());
        anim.SetScalarParameter(s_modName, static_cast<float>(mod));
        anim.Expression(
            static_cast<winrt::hstring>(L"(") + s_inputParameterName + L"." +
            s_valueName + L" + " + s_inputParameterName + L"." + s_offsetName +
            L") % " + s_modName);
        return anim;
      }());
}
} // namespace uwp
} // namespace react
