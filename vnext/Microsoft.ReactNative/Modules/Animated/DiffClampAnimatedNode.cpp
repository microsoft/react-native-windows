// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DiffClampAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {
DiffClampAnimatedNode::DiffClampAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, manager) {
  m_inputNodeTag = static_cast<int64_t>(config[s_inputName].AsDouble());
  m_min = config[s_minName].AsDouble();
  m_max = config[s_maxName].AsDouble();

  m_propertySet.StartAnimation(s_valueName, [node = m_inputNodeTag, min = m_min, max = m_max, manager]() {
    const auto anim = manager->Compositor().CreateExpressionAnimation();
    anim.SetReferenceParameter(s_inputParameterName, manager->GetValueAnimatedNode(node)->PropertySet());
    anim.SetScalarParameter(s_minParameterName, static_cast<float>(min));
    anim.SetScalarParameter(s_maxParameterName, static_cast<float>(max));
    anim.Expression(
        static_cast<winrt::hstring>(L"Clamp(") + s_inputParameterName + L"." + s_valueName + L" + " +
        s_inputParameterName + L"." + s_offsetName + L", " + s_minParameterName + L", " + s_maxParameterName + L")");
    return anim;
  }());
}
} // namespace Microsoft::ReactNative
