// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ModulusAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {
ModulusAnimatedNode::ModulusAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, config, manager) {
  m_inputNodeTag = config[s_inputName].AsInt64();
  assert(HasCompatibleAnimationDriver(m_inputNodeTag));
  m_modulus = config[s_modulusName].AsInt64();

  if (m_useComposition) {
    m_propertySet.StartAnimation(s_valueName, [node = m_inputNodeTag, mod = m_modulus, manager]() {
      const auto anim = manager->Compositor().CreateExpressionAnimation();
      anim.SetReferenceParameter(s_inputParameterName, manager->GetValueAnimatedNode(node)->PropertySet());
      anim.SetScalarParameter(s_modName, static_cast<float>(mod));
      anim.Expression(
          static_cast<winrt::hstring>(L"(") + s_inputParameterName + L"." + s_valueName + L" + " +
          s_inputParameterName + L"." + s_offsetName + L") % " + s_modName);
      return anim;
    }());
  }
}

void ModulusAnimatedNode::Update() {
  assert(!m_useComposition);
  if (const auto manager = m_manager.lock()) {
    if (const auto node = manager->GetValueAnimatedNode(m_inputNodeTag)) {
      RawValue(std::fmod(node->Value(), m_modulus));
    }
  }
}
} // namespace Microsoft::ReactNative
