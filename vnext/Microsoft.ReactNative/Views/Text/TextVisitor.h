// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/ShadowNodeVisitor.h>

namespace Microsoft::ReactNative {

class TextVisitor : public ShadowNodeVisitor {
  using Super = ShadowNodeVisitor;

 protected:
  virtual void VisitCore(ShadowNodeBase *node) override;

  virtual void VisitRawText(ShadowNodeBase *node);

  virtual void VisitText(ShadowNodeBase *node);

  virtual void VisitVirtualText(ShadowNodeBase *node);
};

}; // namespace Microsoft::ReactNative
