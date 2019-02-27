// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "FrameworkElementViewManager.h"

namespace react { namespace uwp {

class ViewShadowNode;

class ViewViewManager : public FrameworkElementViewManager
{
  using Super = FrameworkElementViewManager;
public:
  ViewViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  const char* GetName() const override;

  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  facebook::react::ShadowNode* createShadow() const override;

  void UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap) override;

protected:
  XamlView CreateViewCore(int64_t tag) override;
  void ReplaceView(ViewShadowNode* viewShadowNode, bool useControl);
  void TransferProperties(XamlView oldView, XamlView newView) override;

private:
  void DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic&& eventData);

  XamlView CreateViewPanel(int64_t tag);
  XamlView CreateViewControl(int64_t tag);
};

} }
