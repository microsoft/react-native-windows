// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/Text/TextVisitor.h>
#include <Utils/TextTransform.h>
#include <stack>

namespace Microsoft::ReactNative {

class TextTransformVisitor : public TextVisitor {
  using Super = TextVisitor;

 public:
  TextTransformVisitor() : Super(), m_forceUpdate{true} {}

  TextTransformVisitor(TextTransform parentTransform) : Super(), m_forceUpdate{false} {
    m_textTransforms.push(parentTransform);
  }

 protected:
  void VisitRawText(ShadowNodeBase *node) override;

  void VisitText(ShadowNodeBase *node) override;

  void VisitVirtualText(ShadowNodeBase *node) override;

 private:
  std::stack<TextTransform> m_textTransforms;
  bool m_forceUpdate;

  bool ShouldApplyTransform(TextTransform transform);
  bool Push(TextTransform transform);
  void Pop();
  TextTransform Top();
};

} // namespace Microsoft::ReactNative
