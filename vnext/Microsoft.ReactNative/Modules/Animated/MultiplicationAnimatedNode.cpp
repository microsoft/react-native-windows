// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "MultiplicationAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {
MultiplicationAnimatedNode::MultiplicationAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : ValueAnimatedNode(tag, manager) {
  for (const auto &inputNode : config.find(s_inputName).dereference().second) {
    m_inputNodes.insert(static_cast<int64_t>(inputNode.asDouble()));
  }

  m_propertySet.StartAnimation(s_valueName, [nodes = m_inputNodes, manager]() {
    const auto anim = Microsoft::ReactNative::GetCompositor().CreateExpressionAnimation();

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
} // namespace Microsoft::ReactNative
