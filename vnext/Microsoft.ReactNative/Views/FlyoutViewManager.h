#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react::uwp {

class FlyoutViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  FlyoutViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  facebook::react::ShadowNode *createShadow() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  friend class FlyoutShadowNode;
};

} // namespace react::uwp
