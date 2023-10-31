// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/NativeUIManager.h>
#include <Views/ShadowNodeBase.h>

namespace Microsoft::ReactNative {

class ShadowNodeVisitor {
 public:
  void Visit(ShadowNode *node);

 protected:
  virtual void VisitCore(ShadowNodeBase *node);

  ShadowNode *GetShadowNode(int64_t tag);

  std::shared_ptr<NativeUIManager> EnsureNativeUIManager(ShadowNode *node);

 private:
  std::shared_ptr<NativeUIManager> m_uiManager;
};

}; // namespace Microsoft::ReactNative
