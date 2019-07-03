// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DiffClampAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace react {
namespace uwp {
DiffClampAnimatedNode::DiffClampAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, config, manager) {
  m_inputNodeTag = static_cast<int64_t>(
      config.find(s_inputName).dereference().second.asDouble());
  m_min = config.find(s_minName).dereference().second.asDouble();
  m_max = config.find(s_maxName).dereference().second.asDouble();

  m_propertySet.StartAnimation(
      s_valueName,
      [node = m_inputNodeTag, min = m_min, max = m_max, manager]() {
        const auto anim =
            winrt::Window::Current().Compositor().CreateExpressionAnimation();
        anim.SetReferenceParameter(
            s_inputParameterName,
            manager->GetValueAnimatedNode(node)->PropertySet());
        anim.SetScalarParameter(s_minParameterName, static_cast<float>(min));
        anim.SetScalarParameter(s_maxParameterName, static_cast<float>(max));
        anim.Expression(
            static_cast<winrt::hstring>(L"Clamp(") + s_inputParameterName +
            L"." + s_valueName + L" + " + s_inputParameterName + L"." +
            s_offsetName + L", " + s_minParameterName + L", " +
            s_maxParameterName + L")");
        return anim;
      }());
}
} // namespace uwp
} // namespace react
