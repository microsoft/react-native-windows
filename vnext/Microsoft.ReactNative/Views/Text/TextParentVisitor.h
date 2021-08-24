// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextVisitor.h"

namespace Microsoft::ReactNative {

class TextParentVisitor : public TextVisitor {
  using Super = TextVisitor;

 protected:
  void VisitCore(ShadowNodeBase *node) override;

  void VisitText(ShadowNodeBase *node) override{};
};

} // namespace Microsoft::ReactNative
