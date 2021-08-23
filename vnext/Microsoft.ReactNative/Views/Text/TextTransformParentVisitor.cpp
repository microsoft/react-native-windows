// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextTransformParentVisitor.h"
#include <Views/TextViewManager.h>
#include <Views/VirtualTextViewManager.h>

namespace Microsoft::ReactNative {

void TextTransformParentVisitor::VisitText(ShadowNodeBase *node) {
  textTransform = TextViewManager::GetTextTransformValue(node);
}

void TextTransformParentVisitor::VisitVirtualText(ShadowNodeBase *node) {
  textTransform = static_cast<VirtualTextShadowNode *>(node)->textTransform;
  if (textTransform == TextTransform::Undefined) {
    Super::VisitVirtualText(node);
  }
}

} // namespace Microsoft::ReactNative
