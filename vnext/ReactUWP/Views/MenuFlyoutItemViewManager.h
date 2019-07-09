#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <Views/FrameworkElementViewManager.h>

namespace winrt {
using MenuFlyoutItem = winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem;
}

namespace react {
namespace uwp {

class MenuFlyoutItemViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  MenuFlyoutItemViewManager(
      const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  facebook::react::ShadowNode *createShadow() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      XamlView viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  friend class MenuFlyoutItemShadowNode;

 private:
  winrt::MenuFlyoutItem::Click_revoker m_menuFlyoutItemClickRevoker{};
};

} // namespace uwp
} // namespace react
