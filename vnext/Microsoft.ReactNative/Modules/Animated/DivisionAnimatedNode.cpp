// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DivisionAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {
DivisionAnimatedNode::DivisionAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, manager) {
  for (const auto &inputNode : config[s_inputName].AsArray()) {
    if (m_firstInput == s_firstInputUnset) {
      m_firstInput = static_cast<int64_t>(inputNode.AsDouble());
    } else {
      m_inputNodes.insert(static_cast<int64_t>(inputNode.AsDouble()));
    }
  }

  m_propertySet.StartAnimation(s_valueName, [firstNode = m_firstInput, nodes = m_inputNodes, manager]() {
    const auto anim = manager->Compositor().CreateExpressionAnimation();

    anim.Expression([firstNode, nodes, manager, anim]() {
      anim.SetReferenceParameter(s_baseName, manager->GetValueAnimatedNode(firstNode)->PropertySet());
      winrt::hstring expr = static_cast<winrt::hstring>(L"(") + s_baseName + L"." + s_valueName + L" + " + s_baseName +
          L"." + s_offsetName + L")";
      for (const auto tag : nodes) {
        const auto identifier = L"n" + std::to_wstring(tag);
        anim.SetReferenceParameter(identifier, manager->GetValueAnimatedNode(tag)->PropertySet());
        expr = expr + L" / (" + identifier + L"." + s_valueName + L" + " + identifier + L"." + s_offsetName + L")";
      }
      return expr;
    }());
    return anim;
  }());
}
} // namespace Microsoft::ReactNative
