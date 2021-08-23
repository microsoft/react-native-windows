// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/TextTransform.h>
#include <stack>
#include "TextVisitor.h"

namespace Microsoft::ReactNative {

class TextHighlighterVisitor : public TextVisitor {
  using Super = TextVisitor;
  using Color = std::optional<winrt::Windows::UI::Color>;

 public:
  TextHighlighterVisitor(Color foregroundColor, Color backgroundColor) : Super() {
    m_foregroundColors.push(foregroundColor);
    m_backgroundColors.push(backgroundColor);
  }

  std::vector<xaml::Documents::TextHighlighter> highlighters{};

 protected:
  void VisitRawText(ShadowNodeBase *node) override;

  void VisitVirtualText(ShadowNodeBase *node) override;

 private:
  int m_startIndex{0};
  std::stack<Color> m_foregroundColors;
  std::stack<Color> m_backgroundColors;

  bool RequiresTextHighlighter(Color foregroundColor, Color backgroundColor);
};

} // namespace Microsoft::ReactNative
