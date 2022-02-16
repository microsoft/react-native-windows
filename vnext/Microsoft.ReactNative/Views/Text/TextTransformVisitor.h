// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/TextTransform.h>
#include <stack>
#include "TextVisitor.h"

namespace Microsoft::ReactNative {

class TextTransformVisitor : public TextVisitor {
  using Super = TextVisitor;

 public:
  TextTransformVisitor(TextTransform parentTransform, bool forceUpdate) : Super(), m_forceUpdate{forceUpdate} {
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
};

} // namespace Microsoft::ReactNative
