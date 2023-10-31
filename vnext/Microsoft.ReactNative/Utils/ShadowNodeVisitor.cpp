// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ShadowNodeVisitor.h"
#include <Modules/PaperUIManagerModule.h>

namespace Microsoft::ReactNative {

void ShadowNodeVisitor::Visit(ShadowNode *node) {
  if (!node || !EnsureNativeUIManager(node))
    return;

  VisitCore(static_cast<ShadowNodeBase *>(node));
}

void ShadowNodeVisitor::VisitCore(ShadowNodeBase *node) {
  for (auto childTag : node->m_children) {
    Visit(GetShadowNode(childTag));
  }
}

ShadowNode *ShadowNodeVisitor::GetShadowNode(int64_t tag) {
  return m_uiManager->getHost()->FindShadowNodeForTag(tag);
}

std::shared_ptr<NativeUIManager> ShadowNodeVisitor::EnsureNativeUIManager(ShadowNode *node) {
  if (!m_uiManager) {
    const auto baseNode = static_cast<ShadowNodeBase *>(node);
    m_uiManager = GetNativeUIManager(baseNode->GetViewManager()->GetReactContext()).lock();
  }

  return m_uiManager;
}

} // namespace Microsoft::ReactNative
