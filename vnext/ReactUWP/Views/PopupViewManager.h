// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react {
namespace uwp {

class PopupViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  PopupViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  folly::dynamic GetNativeProps() const override;

  facebook::react::ShadowNode *createShadow() const override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  friend class PopupShadowNode;
};

} // namespace uwp
} // namespace react
