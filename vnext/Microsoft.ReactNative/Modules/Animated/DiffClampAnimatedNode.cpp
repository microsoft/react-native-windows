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
    : ValueAnimatedNode(tag, config, manager) {
  m_inputNodeTag = static_cast<int64_t>(config[s_inputName].AsDouble());
  assert(HasCompatibleAnimationDriver(m_inputNodeTag));
  m_min = config[s_minName].AsDouble();
  m_max = config[s_maxName].AsDouble();

  if (m_useComposition) {
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
}

void DiffClampAnimatedNode::Update() {
  assert(!m_useComposition);
  if (const auto manager = m_manager.lock()) {
    if (const auto node = manager->GetValueAnimatedNode(m_inputNodeTag)) {
      const auto value = node->Value();
      const auto diff = value - m_lastValue;
      m_lastValue = value;
      RawValue(std::clamp(Value() + diff, m_min, m_max));
    }
  }
}
} // namespace Microsoft::ReactNative
