// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextParentVisitor.h"

namespace Microsoft::ReactNative {

void TextParentVisitor::VisitExtensionText(ShadowNodeBase *node) {
  VisitParent(node);
}

void TextParentVisitor::VisitRawText(ShadowNodeBase *node) {
  VisitParent(node);
}

void TextParentVisitor::VisitVirtualText(ShadowNodeBase *node) {
  VisitParent(node);
}

void TextParentVisitor::VisitParent(ShadowNodeBase *node) {
  Visit(GetShadowNode(node->m_parent));
}

} // namespace Microsoft::ReactNative
