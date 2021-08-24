// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextVisitor.h"
#include <Utils/ShadowNodeTypeUtils.h>

namespace Microsoft::ReactNative {

void TextVisitor::Visit(ShadowNode *node) {
  if (!node || !EnsureNativeUIManager(node))
    return;

  const auto baseNode = static_cast<ShadowNodeBase *>(node);
  if (IsTextShadowNode(baseNode)) {
    VisitText(baseNode);
  } else if (IsVirtualTextShadowNode(baseNode)) {
    VisitVirtualText(baseNode);
  } else if (IsRawTextShadowNode(baseNode)) {
    VisitRawText(baseNode);
  } else {
    VisitCore(baseNode);
  }
}

void TextVisitor::VisitCore(ShadowNodeBase *node) {
  for (auto childTag : node->m_children) {
    Visit(GetShadowNode(childTag));
  }
}

void TextVisitor::VisitRawText(ShadowNodeBase *node) {
  VisitCore(node);
}

void TextVisitor::VisitText(ShadowNodeBase *node) {
  VisitCore(node);
}

void TextVisitor::VisitVirtualText(ShadowNodeBase *node) {
  VisitCore(node);
}

ShadowNode *TextVisitor::GetShadowNode(int64_t tag) {
  return m_uiManager->getHost()->FindShadowNodeForTag(tag);
}

std::shared_ptr<NativeUIManager> TextVisitor::EnsureNativeUIManager(ShadowNode *node) {
  if (!m_uiManager) {
    const auto baseNode = static_cast<ShadowNodeBase *>(node);
    m_uiManager = GetNativeUIManager(baseNode->GetViewManager()->GetReactContext()).lock();
  }

  return m_uiManager;
}

} // namespace Microsoft::ReactNative
