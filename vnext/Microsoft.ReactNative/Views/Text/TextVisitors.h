// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextHighlighterVisitor.h"
#include "TextTransformVisitor.h"

namespace Microsoft::ReactNative {
using Color = std::optional<winrt::Windows::UI::Color>;

static inline std::vector<xaml::Documents::TextHighlighter>
GetNestedTextHighlighters(ShadowNode *node, Color foregroundColor, Color backgroundColor) {
  TextHighlighterVisitor visitor{foregroundColor, backgroundColor};
  visitor.Visit(node);
  return visitor.highlighters;
}

static inline void ApplyTextTransformsToChild(ShadowNode *node, TextTransform transform) {
  TextTransformVisitor visitor{transform};
  visitor.Visit(node);
}

static inline void UpdateTextTransformsForAllChildren(ShadowNode* node) {
  TextTransformVisitor visitor;
  visitor.Visit(node);
}

} // namespace Microsoft::ReactNative
