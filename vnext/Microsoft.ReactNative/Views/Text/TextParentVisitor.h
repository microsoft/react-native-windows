// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextVisitor.h"

namespace Microsoft::ReactNative {

class TextParentVisitor : public TextVisitor {
  using Super = TextVisitor;

 protected:
  void VisitExtensionText(ShadowNodeBase *node) override;

  void VisitRawText(ShadowNodeBase *node) override;

  void VisitText(ShadowNodeBase *node) override{};

  void VisitVirtualText(ShadowNodeBase *node) override;

 private:
  void VisitParent(ShadowNodeBase *node);
};

} // namespace Microsoft::ReactNative
