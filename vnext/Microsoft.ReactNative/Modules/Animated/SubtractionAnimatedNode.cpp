// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedNodeManager.h"
#include "SubtractionAnimatedNode.h"

namespace Microsoft::ReactNative {
SubtractionAnimatedNode::SubtractionAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, config, manager) {
  for (const auto &inputNode : config[s_inputName].AsArray()) {
    const auto inputTag = inputNode.AsInt64();
    assert(HasCompatibleAnimationDriver(inputTag));
    if (m_firstInput == s_firstInputUnset) {
      m_firstInput = inputTag;
    } else {
      m_inputNodes.insert(inputTag);
    }
  }

  if (m_useComposition) {
    m_propertySet.StartAnimation(s_valueName, [firstNode = m_firstInput, nodes = m_inputNodes, manager]() {
      const auto anim = manager->Compositor().CreateExpressionAnimation();

      anim.Expression([firstNode, nodes, manager, anim]() {
        anim.SetReferenceParameter(s_baseName, manager->GetValueAnimatedNode(firstNode)->PropertySet());
        winrt::hstring expr = static_cast<winrt::hstring>(L"(") + s_baseName + L"." + s_valueName + L" + " +
            s_baseName + L"." + s_offsetName + L")";
        for (const auto tag : nodes) {
          const auto identifier = L"n" + std::to_wstring(tag);
          anim.SetReferenceParameter(identifier, manager->GetValueAnimatedNode(tag)->PropertySet());
          expr = expr + L" - (" + identifier + L"." + s_valueName + L" + " + identifier + L"." + s_offsetName + L")";
        }
        return expr;
      }());
      return anim;
    }());
  }
}

void SubtractionAnimatedNode::Update() {
  assert(!m_useComposition);
  if (const auto manager = m_manager.lock()) {
    auto rawValue = 0.0;
    if (const auto firstNode = manager->GetValueAnimatedNode(m_firstInput)) {
      rawValue = firstNode->Value();
    }

    for (const auto &tag : m_inputNodes) {
      if (const auto node = manager->GetValueAnimatedNode(tag)) {
        rawValue -= node->Value();
      }
    }

    RawValue(rawValue);
  }
}
} // namespace Microsoft::ReactNative
