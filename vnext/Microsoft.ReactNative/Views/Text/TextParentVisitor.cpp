// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextParentVisitor.h"

namespace Microsoft::ReactNative {

void TextParentVisitor::VisitCore(ShadowNodeBase *node) {
  Visit(GetShadowNode(node->m_parent));
}

} // namespace Microsoft::ReactNative
