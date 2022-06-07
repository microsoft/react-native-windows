// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextHighlighterVisitor.h"
#include "TextHitTestVisitor.h"
#include "TextPropertyChangedParentVisitor.h"
#include "TextTransformParentVisitor.h"
#include "TextTransformVisitor.h"

namespace Microsoft::ReactNative {
using Color = std::optional<winrt::Windows::UI::Color>;

static inline std::vector<xaml::Documents::TextHighlighter>
GetNestedTextHighlighters(ShadowNode *node, Color foregroundColor, Color backgroundColor) {
  TextHighlighterVisitor visitor{foregroundColor, backgroundColor};
  visitor.Visit(node);
  return visitor.highlighters;
}

static inline void ApplyTextTransformToChild(ShadowNode *node) {
  TextTransformParentVisitor parentVisitor;
  parentVisitor.Visit(node);
  TextTransformVisitor visitor{parentVisitor.textTransform, false};
  visitor.Visit(node);
}

static inline void
HitTest(ShadowNode *node, winrt::Microsoft::ReactNative::ReactPointerEventArgs const &args, winrt::Point const &point) {
  TextHitTestVisitor visitor{args, point};
  visitor.Visit(node);
}

static inline void UpdateTextTransformForChildren(ShadowNode *node) {
  TextTransformParentVisitor parentVisitor;
  parentVisitor.Visit(node);
  TextTransformVisitor visitor{parentVisitor.textTransform, true};
  visitor.Visit(node);
}

static inline void NotifyAncestorsTextPropertyChanged(ShadowNode *node, PropertyChangeType type) {
  TextPropertyChangedParentVisitor visitor{type};
  visitor.Visit(node);
}

} // namespace Microsoft::ReactNative
