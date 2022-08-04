// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/TextTransform.h>
#include "TextParentVisitor.h"

namespace Microsoft::ReactNative {

class TextTransformParentVisitor : public TextParentVisitor {
  using Super = TextParentVisitor;

 public:
  TextTransform textTransform{TextTransform::Undefined};

 protected:
  void VisitText(ShadowNodeBase *node) override;

  void VisitVirtualText(ShadowNodeBase *node) override;
};

} // namespace Microsoft::ReactNative
