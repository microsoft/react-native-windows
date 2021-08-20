// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextVisitor.h"
#include <Utils/TransformableText.h>
#include <Views/RawTextViewManager.h>
#include <Views/ShadowNodeBase.h>
#include <Views/TextViewManager.h>
#include <Views/VirtualTextViewManager.h>

namespace Microsoft::ReactNative {

//
// Helpers
//

static inline bool IsNodeType(ShadowNodeBase const *node, wchar_t const *name) {
  return std::wcscmp(node->GetViewManager()->GetName(), name) == 0;
}

static inline bool IsTextShadowNode(ShadowNodeBase const *node) {
  return IsNodeType(node, L"RCTText");
}

static inline bool IsVirtualTextShadowNode(ShadowNodeBase const *node) {
  return IsNodeType(node, L"RCTVirtualText");
}

static inline bool IsRawTextShadowNode(ShadowNodeBase const *node) {
  return IsNodeType(node, L"RCTRawText");
}

//
// TextVisitor
//

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
    VisitExtensionText(baseNode);
  }
}

void TextVisitor::VisitExtensionText(ShadowNodeBase *node) {
  for (auto childTag : node->m_children) {
    Visit(m_uiManager->getHost()->FindShadowNodeForTag(childTag));
  }
}

void TextVisitor::VisitText(ShadowNodeBase *node) {
  for (auto childTag : node->m_children) {
    Visit(m_uiManager->getHost()->FindShadowNodeForTag(childTag));
  }
}

void TextVisitor::VisitVirtualText(ShadowNodeBase *node) {
  for (auto childTag : node->m_children) {
    Visit(m_uiManager->getHost()->FindShadowNodeForTag(childTag));
  }
}

std::shared_ptr<NativeUIManager> TextVisitor::EnsureNativeUIManager(ShadowNode *node) {
  if (!m_uiManager) {
    const auto baseNode = static_cast<ShadowNodeBase *>(node);
    m_uiManager = GetNativeUIManager(baseNode->GetViewManager()->GetReactContext()).lock();
  }

  return m_uiManager;
}

//
// TextTransformVisitor
//

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
