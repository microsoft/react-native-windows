// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>
#include <Views/ViewPanel.h>

namespace react::uwp {

class ViewShadowNode;

class ViewViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ViewViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;

  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  facebook::react::ShadowNode *createShadow() const override;

  // Yoga Layout
  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;
  void TryUpdateView(ViewShadowNode *viewShadowNode, winrt::react::uwp::ViewPanel &pPanel, bool useControl);
};

} // namespace react::uwp
