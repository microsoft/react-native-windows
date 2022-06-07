// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextHitTestVisitor.h"
#include <Views/VirtualTextViewManager.h>
#include "TextHitTestUtils.h"

namespace winrt {
using namespace xaml::Documents;
} // namespace winrt

namespace Microsoft::ReactNative {

void TextHitTestVisitor::VisitCore(ShadowNodeBase *node) {
  for (auto childTag : node->m_children) {
    Visit(GetShadowNode(childTag));

    // Stop recursion if we found the target
    if (m_targetFound) {
      break;
    }
  }
}

void TextHitTestVisitor::VisitRawText(ShadowNodeBase *node) {
  // Only hit test the raw text if there is a pressable ancestor
  if (m_hasPressableAncestor) {
    const auto run = node->GetView().as<winrt::Run>();
    if (TextHitTestUtils::HitTest(run, m_point)) {
      m_targetFound = true;
    }
  }
}

void TextHitTestVisitor::VisitVirtualText(ShadowNodeBase *node) {
  const auto textNode = static_cast<VirtualTextShadowNode *>(node);

  // Update pressable count and set pressable ancestor flag
  const auto hadPressableAncestor = m_hasPressableAncestor;
  if (textNode->isPressable || textNode->IsRegisteredForMouseEvents()) {
    m_pressableCount++;
    m_hasPressableAncestor = true;
  }

  // Capture initial count to determine if node still has pressable descendants
  const auto initialCount = m_pressableCount;

  // Continue recursion if there is a pressable descendant or ancestor
  if (textNode->hasDescendantPressable || m_hasPressableAncestor) {
    // Visit children
    Super::VisitVirtualText(node);

    // Reset pressable descendant state if descendants are no longer pressable
    if (m_pressableCount == initialCount) {
      textNode->hasDescendantPressable = false;
    }

    // If the target is null after target was found, set the target to the
    // current view. This will set the target to the first virtual text
    // ancestor of the target raw text node as well as handle when a descendant
    // view manager sets `pointerEvents` to "none" or "box-none".
    if (m_targetFound && !m_args.Target()) {
      m_args.Target(node->GetView());
    }

    // Invoke view manager OnPointerEvent method for `pointerEvents` behavior
    if (m_args.Target()) {
      node->GetViewManager()->OnPointerEvent(node, m_args);
    }
  }

  // Reset pressable ancestor state
  m_hasPressableAncestor = hadPressableAncestor;
}

} // namespace Microsoft::ReactNative
