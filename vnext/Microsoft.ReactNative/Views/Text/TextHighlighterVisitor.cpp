// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TextHighlighterVisitor.h"
#include <Utils/ValueUtils.h>
#include <Views/RawTextViewManager.h>
#include <Views/TextViewManager.h>
#include <Views/VirtualTextViewManager.h>

namespace winrt {
using namespace xaml::Documents;
} // namespace winrt

namespace Microsoft::ReactNative {

void TextHighlighterVisitor::VisitRawText(ShadowNodeBase *node) {
  const auto textNode = static_cast<RawTextShadowNode *>(node);
  m_startIndex += textNode->GetView().as<winrt::Run>().Text().size();
}

void TextHighlighterVisitor::VisitVirtualText(ShadowNodeBase *node) {
  const auto textNode = static_cast<VirtualTextShadowNode *>(node);
  const auto foregroundColor = textNode->m_foregroundColor;
  const auto backgroundColor = textNode->m_backgroundColor;
  const auto needsHighlighter = RequiresTextHighlighter(foregroundColor, backgroundColor);
  const auto pushedForeground = Push(m_foregroundColors, foregroundColor);
  const auto pushedBackground = Push(m_backgroundColors, backgroundColor);

  const auto startIndex = m_startIndex;
  const auto initialHighlighterCount = highlighters.size();
  Super::VisitVirtualText(node);

  if (needsHighlighter) {
    winrt::TextHighlighter highlighter;
    highlighter.Background(SolidBrushFromColor(m_backgroundColors.top().value()));
    const auto inheritedForegroundColor = m_foregroundColors.top();
    if (inheritedForegroundColor.has_value()) {
      highlighter.Foreground(SolidBrushFromColor(inheritedForegroundColor.value()));
    }
    highlighter.Ranges().Append({startIndex, m_startIndex - startIndex});
    highlighters.push_back(highlighter);
  } else if (highlighters.size() == initialHighlighterCount) {
    textNode->m_hasDescendantBackgroundColor = false;
  }

  if (pushedForeground)
    m_foregroundColors.pop();
  if (pushedBackground)
    m_backgroundColors.pop();
}

bool TextHighlighterVisitor::RequiresTextHighlighter(Color foregroundColor, Color backgroundColor) {
  return backgroundColor || m_backgroundColors.top() && foregroundColor;
}

bool TextHighlighterVisitor::Push(std::stack<Color> &stack, Color color) {
  if (color.has_value()) {
    stack.push(color);
    return true;
  }

  return false;
}

} // namespace Microsoft::ReactNative
