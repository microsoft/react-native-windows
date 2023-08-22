// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextVisitor.h"
#include <Utils/ShadowNodeTypeUtils.h>

namespace Microsoft::ReactNative {

void TextVisitor::VisitCore(ShadowNodeBase *node) {
  if (IsTextShadowNode(node)) {
    VisitText(node);
  } else if (IsVirtualTextShadowNode(node)) {
    VisitVirtualText(node);
  } else if (IsRawTextShadowNode(node)) {
    VisitRawText(node);
  } else {
    Super::VisitCore(node);
  }
}

void TextVisitor::VisitRawText(ShadowNodeBase *node) {
  Super::VisitCore(node);
}

void TextVisitor::VisitText(ShadowNodeBase *node) {
  Super::VisitCore(node);
}

void TextVisitor::VisitVirtualText(ShadowNodeBase *node) {
  Super::VisitCore(node);
}

} // namespace Microsoft::ReactNative
