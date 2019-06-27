// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewPanel.h>
#include <Views/FrameworkElementViewManager.h>

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

  void UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap) override;

  // Yoga Layout
  void SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height) override;

protected:
  XamlView CreateViewCore(int64_t tag) override;
  void TryUpdateView(ViewShadowNode* viewShadowNode, winrt::react::uwp::ViewPanel& pPanel, bool useControl);

private:
  void DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic&& eventData);

  XamlView CreateViewControl(int64_t tag);
};

} }
