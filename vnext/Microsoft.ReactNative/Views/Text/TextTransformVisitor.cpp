// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextTransformVisitor.h"
#include <Utils/TransformableText.h>
#include <Views/RawTextViewManager.h>
#include <Views/TextViewManager.h>
#include <Views/VirtualTextViewManager.h>

namespace Microsoft::ReactNative {

void TextTransformVisitor::VisitRawText(ShadowNodeBase *node) {
  const auto rawTextNode = static_cast<RawTextShadowNode *>(node);
  auto originalText = rawTextNode->originalText;
  auto run = node->GetView().as<winrt::Run>();
  // Set originalText on the raw text node if it hasn't been set yet
  if (originalText.size() == 0) {
    // Lazily setting original text to avoid keeping two copies of all raw text strings
    originalText = run.Text();
    rawTextNode->originalText = originalText;
  }

  run.Text(TransformableText::TransformText(originalText, m_textTransforms.top()));

  if (std::wcscmp(originalText.c_str(), run.Text().c_str()) == 0) {
    // If the transformed text is the same as the original, we no longer need a second copy
    rawTextNode->originalText = winrt::hstring{};
  }
}

void TextTransformVisitor::VisitText(ShadowNodeBase *node) {
  const auto transform = TextViewManager::GetTextTransformValue(node);
  if (ShouldApplyTransform(transform)) {
    const auto pushed = Push(transform);
    Super::VisitText(node);
    if (pushed) {
      Pop();
    }
  }
}

void TextTransformVisitor::VisitVirtualText(ShadowNodeBase *node) {
  const auto textNode = static_cast<VirtualTextShadowNode *>(node);
  const auto transform = textNode->textTransform;
  if (ShouldApplyTransform(transform)) {
    const auto pushed = Push(transform);
    Super::VisitVirtualText(node);
    if (pushed) {
      Pop();
    }
  }
}

bool TextTransformVisitor::ShouldApplyTransform(TextTransform transform) {
  // If the visitor is applied in the context of a textTransfrom prop value
  // change, the m_forceUpdate flag is set to true.
  if (m_forceUpdate) {
    // When the textTransform prop changes, only recurse if at the root node
    // or if the current node is has an undefined textTransform.
    return m_textTransforms.size() == 0 || transform == TextTransform::Undefined;
  } else {
    // When a node is added to the tree, only recurse if the added node is has
    // an undefined textTransform and the parent is not "none" or undefined.
    const auto parentTransform = Top();
    return parentTransform != TextTransform::Undefined && parentTransform != TextTransform::None &&
        transform == TextTransform::Undefined;
  }
}

bool TextTransformVisitor::Push(TextTransform transform) {
  if (m_textTransforms.size() == 0 || transform != TextTransform::Undefined) {
    m_textTransforms.push(transform);
    return true;
  }

  return false;
}

void TextTransformVisitor::Pop() {
  m_textTransforms.pop();
}

TextTransform TextTransformVisitor::Top() {
  return m_textTransforms.size() > 0 ? m_textTransforms.top() : TextTransform::Undefined;
}

} // namespace Microsoft::ReactNative
