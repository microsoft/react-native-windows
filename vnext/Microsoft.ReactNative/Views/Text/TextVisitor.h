// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Documents.h>
#include <Views/ShadowNodeBase.h>

namespace Microsoft::ReactNative {

class TextVisitor {
 public:
  void Visit(ShadowNode *node);

 protected:
  virtual void VisitCore(ShadowNodeBase *node);

  virtual void VisitRawText(ShadowNodeBase *node);

  virtual void VisitText(ShadowNodeBase *node);

  virtual void VisitVirtualText(ShadowNodeBase *node);

  ShadowNode *GetShadowNode(int64_t tag);

 private:
  std::shared_ptr<NativeUIManager> m_uiManager;

  std::shared_ptr<NativeUIManager> EnsureNativeUIManager(ShadowNode *node);
};

}; // namespace Microsoft::ReactNative
