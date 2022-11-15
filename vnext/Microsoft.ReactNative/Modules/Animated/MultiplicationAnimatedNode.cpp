// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "MultiplicationAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {
MultiplicationAnimatedNode::MultiplicationAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, config, manager) {
  for (const auto &inputNode : config[s_inputName].AsArray()) {
    const auto inputTag = inputNode.AsInt64();
    assert(HasCompatibleAnimationDriver(inputTag));
    m_inputNodes.insert(inputTag);
  }

  if (m_useComposition) {
    m_propertySet.StartAnimation(s_valueName, [nodes = m_inputNodes, manager]() {
      const auto anim = manager->Compositor().CreateExpressionAnimation();

      anim.Expression([nodes, manager, anim]() {
        winrt::hstring expr = L"1";
        for (const auto tag : nodes) {
          auto identifier = L"n" + std::to_wstring(tag);
          anim.SetReferenceParameter(identifier, manager->GetValueAnimatedNode(tag)->PropertySet());
          expr = expr + L" * (" + identifier + L"." + s_valueName + L" + " + identifier + L"." + s_offsetName + L")";
        }
        return expr;
      }());
      return anim;
    }());
  }
}

void MultiplicationAnimatedNode::Update() {
  assert(!m_useComposition);
  auto rawValue = 1.0;
  if (const auto manager = m_manager.lock()) {
    for (const auto tag : m_inputNodes) {
      if (const auto node = manager->GetValueAnimatedNode(tag)) {
        rawValue *= node->Value();
      }
    }
  }
  RawValue(rawValue);
}
} // namespace Microsoft::ReactNative
